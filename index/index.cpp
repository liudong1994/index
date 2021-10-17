#include "index.h"


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

int32_t Index::add_doc(uint32_t docid, const std::vector<std::vector>> &targetValues)
{
    // TODO

    return INDEX_OK;
}

int32_t Index::update_doc(uint32_t docid, const std::vector<Target> &targetValues, void* userData)
{
    // TODO
    std::shared_ptr<DocInfo> docInfo;

    auto itrDocno = m_docid2docno.find(docid);
    if (itrDocno == m_docid2docno.end()) {
        // 新doc 生成docno
        docInfo = gen_doc(docid, targetValues, userData);

    } else {
        // 之前存在doc 先删除在添加
        delete_doc(docid);
        docInfo = get_doc(itrDocno->second);
    }

    

    // 加锁替换docid到docno的映射关系, 避免更新时新老doc出现投放间隔
    


    return INDEX_OK;
}

int32_t Index::delete_doc(uint32_t docid)
{
    // TODO

    return INDEX_OK;
}

int32_t Index::search_doc()
{
    // TODO


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
    m_docid2docno[docid] = docno;

    return INDEX_OK;
}

void Index::print_all_doc()
{
    // TODO

}

}

