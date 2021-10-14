#ifndef _INDEX_H_
#define _INDEX_H_

#include "common.h"
#include "targetinfo.h"


namespace index
{

const int32_t INDEX_MAX_DOC = 102400;


class Index
{
public:
    Index();
    ~Index();

    int32_t init(int32_t maxDoc = INDEX_MAX_DOC);

    int32_t update_doc();   // add or update
    int32_t delete_doc();
    int32_t search_doc();

    void print_all_doc();


private:


private:
    int32_t m_maxDoc = 0;

    // key: target  value: all docno bitmap
    std::unordered_map<std::string, TargetInfo> m_allTargetDocnos;

    

};

}

#endif //_INDEX_H_

