#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <vector>
#include "common.h"


namespace dindex
{

class Bitmap
{
public:
    Bitmap(uint32_t maxBit);
    ~Bitmap();

    int32_t init(uint32_t maxBit);

    Bitmap& operator = (const Bitmap &other);
    Bitmap& operator &= (const Bitmap &other);
    Bitmap& operator |= (const Bitmap &other);

    int32_t set_bit(uint32_t index, bool set);
    int32_t get_bit(uint32_t index);

    void get_all_bits(std::vector<uint32_t> &allBits);
    std::vector<uint32_t> get_all_bits();
    void clear_all_bits(int32_t bit);


private:
    uint32_t    m_maxBit = 0;
    std::vector<uint8_t> m_allBits;

    uint32_t    m_usedMaxBit = 0;   // 最大的bit数值 优化遍历size
    uint32_t    m_setBitSize = 0;   // 设置为1的bit数 模糊值
};

}

#endif //_BITMAP_H_

