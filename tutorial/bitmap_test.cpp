#include <stdio.h>
#include <string.h>
#include <vector>
#include "bitmap.h"

using namespace dindex;

int main()
{
    Bitmap bitmap(100000);

    bitmap.set_bit(1000, 1);
    bitmap.set_bit(1001, 1);
    bitmap.set_bit(1002, 1);
    bitmap.set_bit(1006, 1);
    bitmap.set_bit(1007, 1);
    bitmap.set_bit(1008, 1);


    for (int i=999; i<1010; ++i) {
        printf("bitmap get %d set:%d\n", i, bitmap.get_bit(i));
    }

    printf("------ ALL BIT SET ------\n");

    std::vector<uint32_t> bitAlldata;
    bitmap.get_all_bits(bitAlldata);
    for (auto &itr : bitAlldata) {
        printf("bitset: %d\n", itr);
    }

    // bitmap |= &= 
    Bitmap bitmap2(100000);

    bitmap2.set_bit(100, 1);
    bitmap2.set_bit(1001, 1);
    bitmap2.set_bit(102, 1);
    bitmap2.set_bit(106, 1);
    bitmap2.set_bit(107, 1);
    bitmap2.set_bit(1008, 1);

    {
        printf("------ ALL BIT SET |= ------\n");

        Bitmap bitmapTmp1 = bitmap;
        Bitmap bitmapTmp2 = bitmap2;
        bitmapTmp1 |= bitmapTmp2;

        std::vector<uint32_t> bitAlldata;
        bitmapTmp1.get_all_bits(bitAlldata);
        for (auto &itr : bitAlldata) {
            printf("bitset: %d\n", itr);
        }
    }

    {
        printf("------ ALL BIT SET &= ------\n");

        Bitmap bitmapTmp1 = bitmap;
        Bitmap bitmapTmp2 = bitmap2;
        bitmapTmp1 &= bitmapTmp2;

        std::vector<uint32_t> bitAlldata;
        bitmapTmp1.get_all_bits(bitAlldata);
        for (auto &itr : bitAlldata) {
            printf("bitset: %d\n", itr);
        }
    }

    {
        printf("------ ALL BIT SET |= and clear ------\n");

        Bitmap bitmapTmp1 = bitmap;
        Bitmap bitmapTmp2 = bitmap2;
        bitmapTmp2.clear_all_bits();
        bitmapTmp1 |= bitmapTmp2;

        std::vector<uint32_t> bitAlldata;
        bitmapTmp1.get_all_bits(bitAlldata);
        for (auto &itr : bitAlldata) {
            printf("bitset: %d\n", itr);
        }
    }

    return 0;
}

// g++ bitmap_test.cpp ../index/bitmap.cpp -g -obitmap_test -std=c++11 -I../index

