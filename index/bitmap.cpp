#include "bitmap.h"
#include "log.h"


namespace dindex
{

const int32_t BIT_ITEM_SIZE = 8;

#define BIT_INDEX(index)    (index / BIT_ITEM_SIZE)
#define BIT_OFFSET(index)   (index % BIT_ITEM_SIZE)


Bitmap::Bitmap()
{
}

Bitmap::~Bitmap()
{
}

int32_t Bitmap::init(uint32_t maxBit)
{
    m_maxBit = maxBit;
    m_allBits.resize(m_maxBit / BIT_ITEM_SIZE + 1);
    return INDEX_OK;
}

Bitmap& Bitmap::operator = (const Bitmap &other)
{
    for (int32_t index=0; index<m_allBits.size(); ++index) {
        m_allBits[index] = other.m_allBits[index];
    }

    m_usedMaxBit = other.m_usedMaxBit;
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

int32_t Bitmap::get_all_bits(std::vector<uint32_t> &all_bits)
{
    all_bits.reserve(m_setBitSize);

    // 获取所有bit位为1的docno
    for (int32_t index=0; index<=m_usedMaxBit; ++index) {
        if ((m_allBits[BIT_INDEX(index)] >> BIT_OFFSET(index)) & 0x01) {
            all_bits.push_back(index);
        }
    }

    return INDEX_OK;
}

void Bitmap::clear_all_bits()
{
    m_setBitSize = 0;
    m_usedMaxBit = 0;
    // vector数组全部重置为0
    std::fill(m_allBits.begin(), m_allBits.end(), 0);
}

}

