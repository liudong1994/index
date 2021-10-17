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

int32_t TargetInfo::add_doc(std::shared_ptr<DocInfo> docInfo)
{
    // 遍历doc target values 设置对应bit
    for (auto &target : docInfo->m_targets) {
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
            bitmap.set_bit(docInfo->m_docno, 1);
        }
    }

    return INDEX_OK;
}

int32_t TargetInfo::del_doc(DocInfo* docInfo)
{
    // TODO
    // 现将doc设置失效 不然修改bitmap时 会出现定向不准
    

    // 归还docno

    return INDEX_OK;
}

Bitmap &TargetInfo::search(const std::string &targetValue)
{
    // TODO

}


}

