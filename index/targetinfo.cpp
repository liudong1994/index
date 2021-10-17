#include "targetinfo.h"
#include "log.h"


namespace dindex
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
    // TODO
    return INDEX_OK;
}

int32_t TargetInfo::add_doc(DocInfo* doc)
{
    // 遍历doc target values 设置对应bit
    for (auto &target : doc->m_targets) {
        // target values
        for (auto &targetValue : target->m_targetValues) {
            auto itrBitmap = m_allBitmaps.find(targetValue);
            if (itrBitmap == m_allBitmaps.end()) {
                // TODO 加锁 如何保证线程安全 这种新定向进行双buffer切换?
                Bitmap bitmap;
                m_allBitmaps.insert(make_pair(targetValue, bitmap));
                itrBitmap = m_allBitmaps.find(targetValue);
            }

            const Bitmap &bitmap = itrBitmap->second;
            bitmap.set_bit(doc->m_docno, 1);
        }
    }

    // 最后设置doc生效
    m_allDocs[doc->m_docno] = doc;
    return INDEX_OK;
}

int32_t TargetInfo::del_doc(DocInfo* doc)
{
    // TODO
    // 现将doc设置失效 不然修改bitmap时 会出现定向不准
    

    // m_allDocs[docno] = NULL;


    // 归还docno


    return INDEX_OK;
}


}

