#ifndef _TARGET_INFO_H_
#define _TARGET_INFO_H_

#include <memory>
#include <unordered_map>
#include <pthread.h>
#include "target.h"
#include "common.h"
#include "bitmap.h"


namespace dindex
{


// 所有docno的集合
class TargetInfo
{
public:
    TargetInfo(const std::string &targetCode, uint32_t maxDoc);
    ~TargetInfo();

    int32_t add_doc(uint32_t docno, const Target &target);
    int32_t del_doc(uint32_t docno, const Target &target);

    Bitmap &search(const std::string &targetValue);

    void print_all_targetvalue();


private:
    uint32_t    m_maxDoc = 0;
    std::string m_targetCode;

    pthread_rwlock_t m_allbitmapLock = PTHREAD_RWLOCK_INITIALIZER;
    // key:targetvalue  value:docnos bitmap
    std::unordered_map<std::string, Bitmap>    m_allBitmaps;

    Bitmap  m_emptyBitmap;
};

}

#endif //_TARGET_INFO_H_

