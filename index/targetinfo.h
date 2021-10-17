#ifndef _TARGET_INFO_H_
#define _TARGET_INFO_H_

#include <stack>
#include <vector>
#include <unordered_set>
#include <unordered_map>
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

    int32_t init();

    int32_t add_doc(DocInfo *doc);
    int32_t del_doc(DocInfo* doc);


private:
    std::string     m_targetCode;

    // 存储所有的bitno
    // TODO targetcode分段  分段加锁
    std::unordered_map<uint32_t, Bitmap>    m_allBitmaps;
};

}

#endif //_TARGET_INFO_H_

