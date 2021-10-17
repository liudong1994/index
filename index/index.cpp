#include "index.h"
#include "bitmap.h"


namespace dindex
{

Index::Index()
{
}

Index::~Index()
{
}

int32_t Index::init(int32_t maxDoc)
{
    m_maxDocno = maxDoc;
    m_allDocs.resize(m_maxDocno, NULL);
    return INDEX_OK;
}

int32_t Index::add_doc(uint32_t docid, const std::vector<std::vector>> &targetValues, void* userData)
{
    // TODO
    std::shared_ptr<DocInfo> docInfo = gen_doc(docid, targetValues, userData);
    if (INDEX_OK != docid_transfor_docno(docid, docInfo->m_docno)) {
        return INDEX_ERROR;
    }
    m_allDocs[docInfo->m_docno] = docInfo;

    for (auto &target : targetValues) {
        auto itr = m_allTargetDocnos.find(target.m_targetCode);
        if (itr == m_allTargetDocnos.end()) {
            SERVER_LOG_WARN("target code not support:%s", target.m_targetCode.c_str());
            continue;
        }

        TargetInfo &targetInfo = itr->second;
        targetInfo.add_doc(docInfo);
    }

    // 加锁替换docid到docno的映射关系, 避免更新时新老doc出现投放间隔
    m_docidnoMutex.lock();
    m_docid2docno[docInfo->m_docid] = docInfo->m_docno;
    m_docidnoMutex.unlock();

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
        SERVER_LOG_ERROR("update doc add doc:%s failed", docid);
        return INDEX_ERROR;
    }

    if (oldDocno != INDEX_ERROR) {
        // 删除之前存在doc 设置doc生效
        delete_doc(oldDocno);
        m_allDocs[oldDocno] = NULL;
    }

    return INDEX_OK;
}

int32_t Index::delete_doc(uint32_t docid)
{
    m_docidnoMutex.lock();
    auto itrDocno = m_docid2docno.find(docid);
    if (itrDocno == m_docid2docno.end()) {
        m_docidnoMutex.unlock();
        SERVER_LOG_ERROR("delete doc id:%u not found", docid);
        return INDEX_ERROR;
    }
    m_docidnoMutex.unlock();

    return delete_doc(itrDocno->second);
}

int32_t Index::delete_doc(uint32_t docno)
{
    return delete_doc(get_doc(docno));
}

int32_t Index::delete_doc(std::shared_ptr<DocInfo> docInfo)
{
    // TODO



    return INDEX_OK;
}

int32_t Index::search_doc(const std::vector<Target> &targetValues, std::vector<std::shared_ptr<DocInfo>> &docs)
{
    // TODO 线程局部变量 thread_local
    Bitmap docnos;

    // 遍历查询所有定向  同定向做并集 不同定向做交集
    for (auto &target & targetValues) {
        auto itr = m_allTargetDocnos.find(target.m_targetCode);
        if (itr == m_allTargetDocnos.end()) {
            SERVER_LOG_WARN("search target code not support:%s", target.m_targetCode.c_str());
            continue;
        }

        TargetInfo &targetInfo = itr->second;

        Bitmap targetDocnos;
        for (auto &targetValue : targetInfo.m_targetValues) {
            targetDocnos |= targetInfo.search(targetValue);
        }

        docnos &= targetDocnos;
    }

    // 取出最终docInfo  根据docid去重 取最新时间 版本号?
    


    return INDEX_OK;
}

std::shared_ptr<DocInfo>& Index::get_doc(uint32_t docno)
{
    if (docno >= m_maxDocno) {
        SERVER_LOG_ERROR("get doc no:%u > max docno:%u", docno, m_maxDocno);
        return NULL;
    }
    
    return m_allDocs[docno];
}

std::shared_ptr<DocInfo>& Index::gen_doc(uint32_t docid, const std::vector<Target> &targetValues, void *userData)
{
    std::shared_ptr<DocInfo> docInfo = std::make_shared<DocInfo>(docid, userData);

    if (INDEX_OK != docid_transfor_docno(docInfo->m_docid, docInfo->m_docno)) {
        SERVER_LOG_ERROR("transfor docno id:%u failed", docInfo->m_docid);
        return NULL;
    }

    docInfo.set_targets(targetValues);

    return docInfo;
}

int32_t Index::docid_transfor_docno(int32_t docid, int32_t &docno)
{
    if (!m_resueDocnos.empty()) {
        docno = m_resueDocnos.top();
        m_resueDocnos.pop();
        return INDEX_OK;
    }

    if (m_nextDocno >= m_maxDocno) {
        SERVER_LOG_ERROR("get free docno, next no:%u > max docno:%u", m_nextDocno, m_maxDocno);
        return INDEX_ERROR;
    }

    docno = m_nextDocno++;
    return INDEX_OK;
}

void Index::print_all_doc()
{
    // TODO

}

}

