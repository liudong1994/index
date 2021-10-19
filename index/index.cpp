#include "index.h"
#include "bitmap.h"
#include "log.h"
#include <unordered_set>


namespace dindex
{

// todo 配置化
const std::vector<std::string> supportTargets = {
    "100001",
    "100002",
    "100003",
    "100004",
    "100005",
    "100006",
    "100007",
    "100008",
    "100009",
    "100010",
    };

Index::Index()
{
}

Index::~Index()
{
}

int32_t Index::init(uint32_t maxDoc)
{
    m_maxDocno = maxDoc;
    m_allDocs.resize(m_maxDocno, NULL);

    // 支持的targetcode
    for (auto &targetCode : supportTargets) {
        m_allTargetDocnos.insert(std::make_pair(targetCode, TargetInfo(targetCode, m_maxDocno)));
    }
    
    return INDEX_OK;
}

int32_t Index::add_doc(uint32_t docid, const std::vector<Target> &targetValues, void* userData)
{
    std::shared_ptr<DocInfo> docInfo = gen_doc(docid, targetValues, userData);
    for (auto &target : targetValues) {
        auto itr = m_allTargetDocnos.find(target.m_targetCode);
        if (itr == m_allTargetDocnos.end()) {
            SERVER_LOG_WARN("target code not support:%s", target.m_targetCode.c_str());
            continue;
        }

        TargetInfo &targetInfo = itr->second;
        targetInfo.add_doc(docInfo->m_docno, target);
    }

    // 加锁替换docid到docno的映射关系, 避免更新时新老doc出现投放间隔
    m_docidnoMutex.lock();
    m_docid2docno[docInfo->m_docid] = docInfo->m_docno;
    m_docidnoMutex.unlock();

    m_allDocs[docInfo->m_docno] = docInfo;

    return INDEX_OK;
}

int32_t Index::update_doc(uint32_t docid, const std::vector<Target> &targetValues, void* userData)
{
    int32_t oldDocno = INDEX_ERROR;

    m_docidnoMutex.lock();
    auto itrDocno = m_docid2docno.find(docid);
    if (itrDocno != m_docid2docno.end()) {
        oldDocno = itrDocno->second;
    }
    m_docidnoMutex.unlock();

    if (INDEX_OK != add_doc(docid, targetValues, userData)) {
        SERVER_LOG_ERROR("update doc add doc:%u failed", docid);
        return INDEX_ERROR;
    }

    if (oldDocno != INDEX_ERROR) {
        // 删除之前存在doc
        std::shared_ptr<DocInfo> docInfo = get_doc(oldDocno);
        delete_doc(docInfo->m_docid);
    }

    return INDEX_OK;
}

int32_t Index::delete_doc(uint32_t docid)
{
    int docno = INDEX_ERROR;

    m_docidnoMutex.lock();
    auto itrDocno = m_docid2docno.find(docid);
    if (itrDocno == m_docid2docno.end()) {
        m_docidnoMutex.unlock();
        SERVER_LOG_ERROR("delete doc id:%u not found", docid);
        return INDEX_ERROR;
    }
    docno = itrDocno->second;
    m_docid2docno.erase(itrDocno);
    m_docidnoMutex.unlock();

    return delete_doc_inner(get_doc(docno));
}

int32_t Index::delete_doc_inner(std::shared_ptr<DocInfo> docInfo)
{
    m_allDocs[docInfo->m_docno] = NULL;

    for (auto &target : docInfo->m_targets) {
        auto itr = m_allTargetDocnos.find(target.m_targetCode);
        if (itr == m_allTargetDocnos.end()) {
            SERVER_LOG_WARN("target code not support:%s", target.m_targetCode.c_str());
            continue;
        }

        TargetInfo &targetInfo = itr->second;
        targetInfo.del_doc(docInfo->m_docno, target);
    }

    free_docno(docInfo->m_docno);
    return INDEX_OK;
}

int32_t Index::search_doc(const std::vector<Target> &targetValues, std::vector<std::shared_ptr<DocInfo>> &docs)
{
    // todo 线程局部变量thread_local
    Bitmap docnos(m_maxDocno);
    docnos.clear_all_bits(1);

    // 遍历查询所有定向  同定向做并集 不同定向做交集
    for (auto &target : targetValues) {
        auto itr = m_allTargetDocnos.find(target.m_targetCode);
        if (itr == m_allTargetDocnos.end()) {
            SERVER_LOG_WARN("search target code not support:%s", target.m_targetCode.c_str());
            continue;
        }

        TargetInfo &targetInfo = itr->second;

        Bitmap targetDocnos(m_maxDocno);
        for (auto &targetValue : target.m_targetValues) {
            targetDocnos |= targetInfo.search(targetValue);
        }

        docnos &= targetDocnos;
    }

    // 取出最终docInfo  根据docid去重 取最新时间 版本号?
    std::vector<uint32_t> &&allDocnos = docnos.get_all_bits();
    docs.reserve(allDocnos.size());

    for (auto &docno : allDocnos) {
        std::shared_ptr<DocInfo> docInfo = get_doc(docno);
        if (docInfo.get() != NULL) {
            docs.push_back(docInfo);
        }

        // todo 更新时 出现多个docid 去重
    }

    return INDEX_OK;
}

std::shared_ptr<DocInfo> Index::get_doc(uint32_t docno)
{
    if (docno >= m_maxDocno) {
        SERVER_LOG_ERROR("get doc no:%u > max docno:%u", docno, m_maxDocno);
        return NULL;
    }
    
    return m_allDocs[docno];
}

std::shared_ptr<DocInfo> Index::gen_doc(uint32_t docid, const std::vector<Target> &targetValues, void *userData)
{
    std::shared_ptr<DocInfo> docInfo = std::make_shared<DocInfo>(docid, userData);

    if (INDEX_OK != get_docno(docInfo->m_docno)) {
        SERVER_LOG_ERROR("transfor docno id:%u failed", docInfo->m_docid);
        return NULL;
    }

    docInfo->set_targets(targetValues);

    return docInfo;
}

int32_t Index::get_docno(uint32_t &docno)
{
    if (!m_resueDocnos.empty()) {
        docno = m_resueDocnos.top();
        m_resueDocnos.pop();
        return INDEX_OK;
    }

    // 使用一遍 全部放到reuseDocnos中 再为空则使用完毕
    if (m_nextDocno >= m_maxDocno) {
        SERVER_LOG_ERROR("get free docno, next no:%u > max docno:%u", m_nextDocno, m_maxDocno);
        return INDEX_ERROR;
    }

    docno = m_nextDocno++;
    return INDEX_OK;
}

void Index::free_docno(uint32_t docno)
{
    m_resueDocnos.push(docno);
}

void Index::print_all_doc()
{
    // all docs
    SERVER_LOG_INFO("======== DOC INFO ========");
    for (auto &docInfo : m_allDocs) {
        if (docInfo.get() == NULL) {
            continue;
        }

        SERVER_LOG_INFO("docid:%u, docno:%u", docInfo->m_docid, docInfo->m_docno);

        for (auto &target : docInfo->m_targets) {
            std::string targetValues("");
            for (auto &value : target.m_targetValues) {
                targetValues += value + ",";
            }

            SERVER_LOG_INFO("target code:%s value:%s", target.m_targetCode.c_str(), targetValues.c_str());
        }
    }

    // all targets
    SERVER_LOG_INFO("======== TARGET INFO ========");
    for (auto itr=m_allTargetDocnos.begin(); itr!=m_allTargetDocnos.end(); ++itr) {
        TargetInfo &targetInfo = itr->second;
        targetInfo.print_all_targetvalue();
    }

    return ;
}

}

