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

int32_t TargetInfo::add_doc(uint32_t docno, const Target &target)
{
    // 遍历target values 设置对应bit
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
        bitmap.set_bit(docno, 1);
    }

    return INDEX_OK;
}

int32_t TargetInfo::del_doc(uint32_t docno, const Target &target)
{
    // 遍历doc target values 设置对应bit
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
        bitmap.set_bit(docno, 0);
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

void TargetInfo::print_all_targetvalue()
{
    SERVER_LOG_INFO("target code:%s", m_targetCode.c_str());

    for (auto itr=m_allBitmaps.begin(); itr!=m_allBitmaps.end(); ++itr) {
        Bitmap &bitmap = itr->second;
        
        std::string docnos;
        std::vector<uint32_t> &&allDocnos = bitmap.get_all_bits();
        for (auto &docno : allDocnos) {
            docnos += std::to_string(docno) + ",";
        }
        
        SERVER_LOG_INFO("target value:%s, docnos:%s", itr->first.c_str(), docnos.c_str());
    }

    return ;
}

}

