#ifndef _TARGET_INFO_H_
#define _TARGET_INFO_H_

#include <memory>
#include <unordered_map>
#include <pthread.h>
#include "common.h"
#include "bitmap.h"


namespace dindex
{

struct DocInfo;

// 所有docno的集合
class TargetInfo
{
public:
    TargetInfo(const std::string &targetCode, uint32_t maxDoc);
    ~TargetInfo();

    int32_t add_doc(std::shared_ptr<DocInfo> docInfo);
    int32_t del_doc(std::shared_ptr<DocInfo> docInfo);

    Bitmap &&search(const std::string &targetValue);


private:
    uint32_t    m_maxDoc = 0;
    std::string m_targetCode;

    pthread_rwlock_t m_allbitmapLock = PTHREAD_RWLOCK_INITIALIZER;
    // key:targetvalue  value:docnos bitmap
    std::unordered_map<std::string, Bitmap>    m_allBitmaps;
};

}

#endif //_TARGET_INFO_H_

