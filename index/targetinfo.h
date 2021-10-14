#ifndef _TARGET_INFO_H_
#define _TARGET_INFO_H_

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "common.h"
#include "bitmap.h"


namespace index
{

struct DocInfo;

struct Target
{
    std::string m_targetCode;
    std::unordered_set<std::string> m_targetValues;
};


// 所有docno的集合
class TargetInfo
{
public:
    TargetInfo(const std::string &targetCode, uint32_t maxDoc);
    ~TargetInfo();

    int32_t init();

    void set_doc(DocInfo *doc);
    DocInfo* get_doc(uint32_t docno);


private:
    int32_t docid_transfor_docno(uint32_t docid);


private:
    uint32_t        m_maxDocno = 0;
    std::string     m_targetCode;

    // key: docno  value: bitno
    std::unordered_map<uint32_t, uint32_t>  m_docno2bitno;

    // 存储所有的bitno
    // bool    m_allBitmapUseFirst = true;  TODO 两个map如何同步
    std::unordered_map<uint32_t, Bitmap>    m_allBitmaps;
    std::unordered_map<uint32_t, Bitmap>    m_allBitmapsSecond;

    std::vector<DocInfo*>  m_allDocs;
};

}

#endif //_TARGET_INFO_H_

