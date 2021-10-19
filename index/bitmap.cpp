#include "bitmap.h"
#include "log.h"


namespace dindex
{

const int32_t BIT_ITEM_SIZE = 8;

#define BIT_INDEX(index)    (index / BIT_ITEM_SIZE)
#define BIT_OFFSET(index)   (index % BIT_ITEM_SIZE)


Bitmap::Bitmap(uint32_t maxBit)
: m_maxBit(maxBit)
{
    m_allBits.resize(m_maxBit / BIT_ITEM_SIZE + 1);
}

Bitmap::~Bitmap()
{
}

Bitmap& Bitmap::operator = (const Bitmap &other)
{
    for (uint32_t index=0; index<m_allBits.size(); ++index) {
        m_allBits[index] = other.m_allBits[index];
    }

    m_usedMaxBit = other.m_usedMaxBit;
    m_setBitSize = other.m_setBitSize;
    return *this;
}

Bitmap& Bitmap::operator &= (const Bitmap &other)
{
    int32_t minUseBit = m_usedMaxBit < other.m_usedMaxBit ? m_usedMaxBit : other.m_usedMaxBit;
    int32_t indexSize = BIT_INDEX(minUseBit) + 1;
    for (int32_t index=0; index<indexSize; ++index) {
        m_allBits[index] &= other.m_allBits[index];
    }

    m_usedMaxBit = minUseBit;
    m_setBitSize = minUseBit;
    return *this;
}

Bitmap& Bitmap::operator |= (const Bitmap &other)
{
    int32_t maxUseBit = m_usedMaxBit > other.m_usedMaxBit ? m_usedMaxBit : other.m_usedMaxBit;
    int32_t indexSize = BIT_INDEX(maxUseBit) + 1;
    for (int32_t index=0; index<indexSize; ++index) {
        m_allBits[index] |= other.m_allBits[index];
    }

    m_usedMaxBit = maxUseBit;
    m_setBitSize = maxUseBit;
    return *this;
}

int32_t Bitmap::set_bit(uint32_t index, bool set)
{
    if (index >= m_maxBit) {
        SERVER_LOG_ERROR("set bit index:%d > maxbit:%d failed", index, m_maxBit);
        return INDEX_ERROR;
    }

    if (set) {
        // 设置标志位 或 当前位为1
        if (!get_bit(index)) {
            m_setBitSize++;
        }
        m_allBits[BIT_INDEX(index)] |= (1 << BIT_OFFSET(index));

    } else {
        // 清除标志位 与 当前位为0
        if (get_bit(index)) {
            m_setBitSize--;
        }
        m_allBits[BIT_INDEX(index)] &= ~(1 << BIT_OFFSET(index));
    }

    if (index > m_usedMaxBit) {
        m_usedMaxBit = index;
    }

    return INDEX_OK;
}

int32_t Bitmap::get_bit(uint32_t index)
{
    if (index >= m_maxBit) {
        SERVER_LOG_ERROR("get bit index:%d > maxbit:%d failed", index, m_maxBit);
        return INDEX_ERROR;
    }

    return (m_allBits[BIT_INDEX(index)] >> BIT_OFFSET(index)) & 0x01;
}

void Bitmap::get_all_bits(std::vector<uint32_t> &allBits)
{
    allBits.reserve(m_setBitSize);

    // 获取所有bit位为1的docno
    for (uint32_t index=0; index<=m_usedMaxBit; ++index) {
        if ((m_allBits[BIT_INDEX(index)] >> BIT_OFFSET(index)) & 0x01) {
            allBits.push_back(index);
        }
    }

    return ;
}

std::vector<uint32_t> Bitmap::get_all_bits()
{
    std::vector<uint32_t> allBits;
    allBits.reserve(m_setBitSize);

    // 获取所有bit位为1的docno
    for (uint32_t index=0; index<=m_usedMaxBit; ++index) {
        if ((m_allBits[BIT_INDEX(index)] >> BIT_OFFSET(index)) & 0x01) {
            allBits.push_back(index);
        }
    }

    return allBits;
}

void Bitmap::clear_all_bits(int32_t bit)
{
    if (bit) {
        m_setBitSize = m_maxBit;
        m_usedMaxBit = m_maxBit;
        // vector数组全部重置为255
        std::fill(m_allBits.begin(), m_allBits.end(), 255);

    } else {
        m_setBitSize = 0;
        m_usedMaxBit = 0;
        // vector数组全部重置为0
        std::fill(m_allBits.begin(), m_allBits.end(), 0);
    }
}

}

