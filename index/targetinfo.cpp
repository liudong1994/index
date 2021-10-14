#include "targetinfo.h"
#include "log.h"


namespace index
{

TargetInfo::TargetInfo(const std::string &targetCode, int maxDoc)
: m_targetCode(targetCode)
: m_maxDocno(maxDoc)
{
}

TargetInfo::~TargetInfo()
{
}

int32_t TargetInfo::init()
{
    m_allDocs.resize(m_maxDocno, NULL);

    return INDEX_OK;
}

int32_t TargetInfo::set_doc(DocInfo* doc)
{
    // 根据docid找到docno
    int32_t docno = docid_transfor_docno(doc->m_docid);
    if (INDEX_ERROR == docno) {
        SERVER_LOG_ERROR("set docid:%u failed", doc->m_docid);
        return INDEX_ERROR;
    }

    // 遍历doc target values 设置对应bit
    for (auto &target : doc->m_targets) {
        // target values
        for (auto &targetValue : target->m_targetValues) {
            auto itrBitmap = m_allBitmaps.find(targetValue);
            if (itrBitmap == m_allBitmaps.end()) {
                Bitmap bitmap;
                m_allBitmaps.insert(make_pair(targetValue, bitmap));    // TODO 如何保证线程安全  这种新定向进行双buffer切换?
                itrBitmap = m_allBitmaps.find(targetValue);
            }

            const Bitmap &bitmap = itrBitmap->second;
            bitmap.setbit(docno, 1);
        }
    }

    m_allDocs[docno] = doc;
    return INDEX_OK;
}

DocInfo* TargetInfo::get_doc(uint32_t docno)
{
    if (docno >= m_maxDocno) {
        SERVER_LOG_ERROR("get doc no:%u large max docno:%u", docno, m_maxDocno);
        return NULL;
    }
    
    return m_allDocs[docno];
}

int32_t TargetInfo::docid_transfor_docno(int32_t docid)
{
    

}

}

