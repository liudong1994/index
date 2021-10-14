#ifndef _DOC_INFO_H_
#define _DOC_INFO_H_

#include <vector>
#include <unordered_map>
#include "common.h"
#include "targetinfo.h"


namespace index
{

struct DocInfo
{
    // docid
    uint32_t m_docid = 0;
    // doc对应所有的定向
    std::vector<Target> m_targets;
    // 用户数据
    void *m_userData = NULL;
};

}

#endif //_DOC_INFO_H_

