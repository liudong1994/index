#include <stdio.h>
#include <string.h>
#include <thread>
#include <vector>
#include "index.h"

using namespace dindex;

int main()
{
    // 可以对没有指定targetcode的定向 使用ALL定向填充

    Index index;
    index.init(1024);

    for (int i=0; i<10; ++i) {
        std::vector<Target> targetValues;

        for (int j=0; j<5; ++j) {
            int targetCode = 100001 + rand() % 5;

            Target target;
            target.m_targetCode = std::to_string(targetCode);
            target.m_targetValues.insert( std::to_string(targetCode * 1000 + rand() % 20) );
            target.m_targetValues.insert( std::to_string(targetCode * 1000 + rand() % 20) );
            target.m_targetValues.insert( std::to_string(targetCode * 1000 + rand() % 20) );
            targetValues.push_back(target);
        }

        index.add_doc(i, targetValues, NULL);
    }

    index.print_all_doc();

    std::vector<std::thread> searchThreads;
    for (int i=0; i<10; ++i) {
        searchThreads.emplace_back(
            std::thread([&]() {
                int targetCode = 100001 + rand() % 5;

                Target target;
                target.m_targetCode = std::to_string(targetCode);
                target.m_targetValues.insert( std::to_string(targetCode * 1000 + rand() % 20) );

                std::vector<Target> targetValues;
                targetValues.push_back(target);

                struct timeval on, off;
                for (int i=0; i<10; ++i) {
                    gettimeofday(&on, NULL);

                    std::vector<std::shared_ptr<DocInfo>> docs;
                    index.search_doc(targetValues, docs);

                    gettimeofday(&off, NULL);
                    printf("search doc use time us:%u\n", ( off.tv_sec * 1000000 + off.tv_usec ) - ( on.tv_sec * 1000000 + on.tv_usec ));
                }

                /*printf("search result:\n");
                for (auto &doc : docs) {
                    printf("search docid:%u\n", doc->m_docid);
                }*/
            })
        );
    }

    for (auto &thread : searchThreads) {
        thread.join();
    }

    return 0;
}

// g++ index_tutorial.cpp -g -oindex_tutorial -std=c++11 -ldindex -lpthread -ldl -L../lib -I../index

