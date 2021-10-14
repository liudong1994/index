#include "bitmap.h"
#include "log.h"


namespace index
{

const int32_t BIT_ITEM_SIZE = 32;

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
        m_allBits[BIT_INDEX] |= (1 << BIT_OFFSET);

    } else {
        // 清除标志位 与 当前位为0
        m_allBits[BIT_INDEX] &= ~(1 << BIT_OFFSET);
    }

    if (index > m_usedMaxBit) {
        m_usedMaxBit = index;
    }

    return INDEX_OK;
}

int32_t Bitmap::Bitmap::get_bit(uint32_t index)
{
    if (index >= m_maxBit) {
        SERVER_LOG_ERROR("get bit index:%d > maxbit:%d failed", index, m_maxBit);
        return INDEX_ERROR;
    }

    return m_allBits[BIT_INDEX] & (1 << BIT_OFFSET);
}

int32_t Bitmap::get_all_bits(vector<uint32_t> &all_bits)
{
    // TODO 获取所有bit位为1的docno
    int32_t bitIndex = 0;
    all_bits.reserve(BIT_INDEX(m_usedMaxBit) + 1);

    for (int32_t index=0; index<=m_usedMaxBit; ++index) {
        if ((m_allBits[BIT_INDEX(index)] >> BIT_OFFSET(index)) & 0x01) {
            all_bits[bitIndex++] = index;
        }
    }

    return INDEX_OK;
}

void Bitmap::clear_all_bits()
{
    m_usedMaxBit = 0;
    m_allBits.resize(m_allBits.size(), 0);
}

}

