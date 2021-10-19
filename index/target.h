#ifndef _TARGET_H_
#define _TARGET_H_

#include <string>
#include <unordered_set>


namespace dindex
{

struct Target
{
    std::string m_targetCode;
    std::unordered_set<std::string> m_targetValues;
};

}

#endif //_TARGET_H_

