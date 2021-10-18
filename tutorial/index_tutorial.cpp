#include <stdio.h>
#include <string.h>
#include <thread>
#include <vector>
#include "index.h"

using namespace dindex;

int main()
{
    // TODO
    // 对没有指定targetcode的定向 使用ALL定向填充

    Index index;
    index.init(1024);

    {
        Target target;
        target.m_targetCode = "100001";
        target.m_targetValues.insert("100001001");
        target.m_targetValues.insert("100001002");
        target.m_targetValues.insert("100001003");

        std::vector<Target> targetValues;
        targetValues.push_back(target);

        index.add_doc(10, targetValues, NULL);
    }

    {
        Target target;
        target.m_targetCode = "100002";
        target.m_targetValues.insert("100002001");
        target.m_targetValues.insert("100002002");
        target.m_targetValues.insert("100002003");

        std::vector<Target> targetValues;
        targetValues.push_back(target);

        index.add_doc(100, targetValues, NULL);
    }

    {
        Target target;
        target.m_targetCode = "100002";
        target.m_targetValues.insert("100002001");
        std::vector<Target> targetValues;
        targetValues.push_back(target);

        std::vector<std::shared_ptr<DocInfo>> docs;
        index.search_doc(targetValues, docs);

        printf("search result:\n");
        for (auto &doc : docs) {
            printf("search docid:%u", doc->m_docid);
        }
    }

    return 0;
}

// g++ index_tutorial.cpp -g -oindex_tutorial -std=c++11 -ldindex -lpthread -ldl -L../lib -I../index

