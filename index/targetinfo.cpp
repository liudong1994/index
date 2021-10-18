#include "targetinfo.h"
#include "index.h"
#include "log.h"


namespace dindex
{

TargetInfo::TargetInfo(const std::string &targetCode, uint32_t maxDoc)
: m_maxDoc(maxDoc)
, m_targetCode(targetCode)
, m_emptyBitmap(maxDoc)
{
}

TargetInfo::~TargetInfo()
{
}

int32_t TargetInfo::add_doc(std::shared_ptr<DocInfo> docInfo)
{
    // 遍历doc target values 设置对应bit
    for (auto &target : docInfo->m_targets) {
        // target values
        for (auto &targetValue : target.m_targetValues) {
            pthread_rwlock_rdlock(&m_allbitmapLock);
            auto itrBitmap = m_allBitmaps.find(targetValue);
            if (itrBitmap == m_allBitmaps.end()) {
                pthread_rwlock_unlock(&m_allbitmapLock);

                pthread_rwlock_wrlock(&m_allbitmapLock);
                m_allBitmaps.insert(std::make_pair(targetValue, Bitmap(m_maxDoc)));
                pthread_rwlock_unlock(&m_allbitmapLock);

                itrBitmap = m_allBitmaps.find(targetValue);

            } else {
                pthread_rwlock_unlock(&m_allbitmapLock);
            }

            Bitmap &bitmap = itrBitmap->second;
            bitmap.set_bit(docInfo->m_docno, 1);
        }
    }

    return INDEX_OK;
}

int32_t TargetInfo::del_doc(std::shared_ptr<DocInfo> docInfo)
{
    // 遍历doc target values 设置对应bit
    for (auto &target : docInfo->m_targets) {
        // target values
        for (auto &targetValue : target.m_targetValues) {
            pthread_rwlock_rdlock(&m_allbitmapLock);
            auto itrBitmap = m_allBitmaps.find(targetValue);
            if (itrBitmap == m_allBitmaps.end()) {
                SERVER_LOG_WARN("targetinfo del value:%s not found", targetValue.c_str());
                pthread_rwlock_unlock(&m_allbitmapLock);
                continue;
            }
            pthread_rwlock_unlock(&m_allbitmapLock);

            Bitmap &bitmap = itrBitmap->second;
            bitmap.set_bit(docInfo->m_docno, 0);
        }
    }

    return INDEX_OK;
}

Bitmap &TargetInfo::search(const std::string &targetValue)
{
    pthread_rwlock_rdlock(&m_allbitmapLock);
    auto itrBitmap = m_allBitmaps.find(targetValue);
    pthread_rwlock_unlock(&m_allbitmapLock);

    if (itrBitmap == m_allBitmaps.end()) {
        return m_emptyBitmap;
    }
    return itrBitmap->second;
}

}

