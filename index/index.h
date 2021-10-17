#ifndef _INDEX_H_
#define _INDEX_H_

#include <memory>
#include "common.h"
#include "targetinfo.h"


namespace dindex
{

const int32_t INDEX_MAX_DOC = 102400;


struct Target
{
    std::string m_targetCode;
    std::unordered_set<std::string> m_targetValues;
};

struct DocInfo
{
    // docid
    uint32_t m_docid = 0;
    // 用户数据
    void *m_userData = NULL;
    // doc对应所有的定向
    std::vector<Target> m_targets;

    // 根据docid计算的docno
    uint32_t m_docno = 0;


    DocInfo(uint32_t docid, void* userData): m_docid(docid), m_userData(userData)
    {
    }

    void set_targets(std::vector<Target> &targets)
    {
        m_targets.clear();
        m_targets.reserve(targets.size());

        m_targets.assign(targets.begin(), targets.end());
    }
    
};

class Index
{
public:
    Index();
    ~Index();

    int32_t init(int32_t maxDoc = INDEX_MAX_DOC);

    int32_t add_doc(uint32_t docid, const std::vector<std::vector>> &targetValues);
    int32_t update_doc(uint32_t docid, const std::vector<std::vector>> &targetValues);
    int32_t delete_doc(uint32_t docid);
    int32_t search_doc(const std::vector<std::vector>> &targetValues);

    void print_all_doc();


private:
    int32_t docid_transfor_docno(uint32_t docid, int32_t &docno);
    std::shared_ptr<DocInfo>& gen_doc(uint32_t docid, const std::vector<Target> &targetValues, void *userData);

    std::shared_ptr<DocInfo>& get_doc(uint32_t docno);


private:
    // key: target  value: all docno bitmap
    std::unordered_map<std::string, TargetInfo> m_allTargetDocnos;

    
    // 提出到index.cpp  target只处理定向相关bitmap

    // key:docid value:docno
    std::unordered_map<uint32_t, uint32_t>  m_docid2docno;

    // 存储所有的doc信息
    uint32_t        m_maxDocno = 0;
    std::vector<std::shared_ptr<DocInfo>>   m_allDocs;
    // 辅助: 根据docid转换为docno
    std::stack<uint32_t>    m_resueDocnos;  // 可复用的docno 优先使用
    uint32_t                m_nextDocno;    // 当可复用docno为空时 优先使用当前docno

    // std::list<DocInfo*>     m_freeDocs; // 延时释放的doc TODO 智能指针代替

};

}

#endif //_INDEX_H_

