#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <vector>
#include "common.h"


namespace index
{

class Bitmap
{
public:
    Bitmap();
    ~Bitmap();

    int32_t init(uint32_t maxBit);

    Bitmap& operator = (const Bitmap &other);
    Bitmap& operator &= (const Bitmap &other);
    Bitmap& operator |= (const Bitmap &other);


    int32_t set_bit(uint32_t index, bool set);
    int32_t get_bit(uint32_t index);

    int32_t get_all_bits(vector<uint32_t> &all_bits);
    void clear_all_bits();


private:
    uint32_t    m_maxBit = 0;
    std::vector<uint32_t> m_allBits;

    uint32_t    m_usedMaxBit = 0;
};

}

#endif //_BITMAP_H_

