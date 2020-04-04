#include <gtest/gtest.h>

#include "test-utils.hpp"

extern "C" {
#include "fat.h"
}

TEST(InitFatTest, HandlesFatImg) {
    unsigned char ramFDD144[BLOCKSIZE * 19];
    size_t cnt = loadFile("test/disk/fat.img", ramFDD144);
    ASSERT_EQ(cnt, BLOCKSIZE * 19)
        << "Can't find the disk file or file size doesn't match";

    int res = initFat(ramFDD144);
    ASSERT_EQ(res, 0) << "Init error";

    const unsigned char *fat = getFat();
    size_t fat1BaseSec = 1, fat2BaseSec = 10;
    for (size_t secOfst = 0; secOfst < 9; secOfst++)
        for (size_t offset = 0; offset < BLOCKSIZE; offset++) {
            EXPECT_EQ(ramFDD144[(fat1BaseSec + secOfst) * BLOCKSIZE + offset],
                      fat[secOfst * BLOCKSIZE + offset]);
            EXPECT_EQ(ramFDD144[(fat2BaseSec + secOfst) * BLOCKSIZE + offset],
                      fat[secOfst * BLOCKSIZE + offset]);
        }
}

TEST(WriteFatsTest, HandlesFatImg) {
    unsigned char ramFDD144[BLOCKSIZE * 19];
    size_t cnt = loadFile("test/disk/fat.img", ramFDD144);
    ASSERT_EQ(cnt, BLOCKSIZE * 19)
        << "Can't find the disk file or file size doesn't match";
    int res = initFat(ramFDD144);
    ASSERT_EQ(res, 0) << "Init error";

    EXPECT_EQ(getFreeClus(), 0x81f);
    for (size_t baseClus = 0x81f, clusOfst = 0; clusOfst < 2847 - 0x81d;
         clusOfst++)
        setFatClus(baseClus + clusOfst, 0xfff);
    writeFats(ramFDD144);

    size_t fat1Base = BLOCKSIZE, fat2Base = 10 * BLOCKSIZE;
    for (size_t clus = 2; clus < BLOCKNUM - 31; clus++) {
        unsigned short offset = clus / 2 * 3;
        unsigned short nextClus1, nextClus2;
        if (clus % 2 == 0) {
            nextClus1 = ((ramFDD144[fat1Base + offset + 1] & 0x0f) << 8) |
                        ramFDD144[fat1Base + offset];
            nextClus2 = ((ramFDD144[fat2Base + offset + 1] & 0x0f) << 8) |
                        ramFDD144[fat2Base + offset];
        } else {
            nextClus1 = (ramFDD144[fat1Base + offset + 2] << 4) |
                        ((ramFDD144[fat1Base + offset + 1] >> 4) & 0x0f);
            nextClus2 = (ramFDD144[fat2Base + offset + 2] << 4) |
                        ((ramFDD144[fat2Base + offset + 1] >> 4) & 0x0f);
        }
        EXPECT_EQ(nextClus1, getNextClus(clus));
        EXPECT_EQ(nextClus2, getNextClus(clus));
    }
}

TEST(GetNextClusTest, HandlesIOSYS) {
    unsigned char ramFDD144[BLOCKSIZE * 19];
    size_t cnt = loadFile("test/disk/fat.img", ramFDD144);
    ASSERT_EQ(cnt, BLOCKSIZE * 19)
        << "Can't find the disk file or file size doesn't match";
    int res = initFat(ramFDD144);
    ASSERT_EQ(res, 0) << "Init error";
    for (unsigned short clus = 0x002; clus != 0xfff; clus = getNextClus(clus))
        if (clus != 0x051)
            EXPECT_EQ(getNextClus(clus), clus + 1);
        else
            EXPECT_EQ(getNextClus(clus), 0xfff);
}

TEST(GetFreeClusTest, HandlesGetOneFreeClus) {
    unsigned char ramFDD144[BLOCKSIZE * 19];
    size_t cnt = loadFile("test/disk/fat.img", ramFDD144);
    ASSERT_EQ(cnt, BLOCKSIZE * 19)
        << "Can't find the disk file or file size doesn't match";
    int res = initFat(ramFDD144);
    ASSERT_EQ(res, 0) << "Init error";
    EXPECT_EQ(getFreeClus(), 0x81f);
}

TEST(GetFreeClusTest, HandlesNoFreeClus) {
    unsigned char ramFDD144[BLOCKSIZE * 19];
    size_t cnt = loadFile("test/disk/fat.img", ramFDD144);
    ASSERT_EQ(cnt, BLOCKSIZE * 19)
        << "Can't find the disk file or file size doesn't match";
    int res = initFat(ramFDD144);
    ASSERT_EQ(res, 0) << "Init error";

    EXPECT_EQ(getFreeClus(), 0x81f);
    for (size_t baseClus = 0x81f, clusOfst = 0; clusOfst < 2847 - 0x81d;
         clusOfst++)
        setFatClus(baseClus + clusOfst, 0xfff);
    EXPECT_EQ(getFreeClus(), (unsigned short)-1);
}

TEST(SetFatClusTest, HandlesFatImg) {
    unsigned char ramFDD144[BLOCKSIZE * 19];
    size_t cnt = loadFile("test/disk/fat.img", ramFDD144);
    ASSERT_EQ(cnt, BLOCKSIZE * 19)
        << "Can't find the disk file or file size doesn't match";
    int res = initFat(ramFDD144);
    ASSERT_EQ(res, 0) << "Init error";

    EXPECT_EQ(getFreeClus(), 0x81f);
    setFatClus(0x81f, 0xfff);
    EXPECT_EQ(getNextClus(0x81f), 0xfff);
    EXPECT_EQ(getFreeClus(), 0x820);
    setFatClus(0x81f, 0x820);
    setFatClus(0x820, 0xfff);
    EXPECT_EQ(getNextClus(0x81f), 0x820);
    EXPECT_EQ(getNextClus(0x820), 0xfff);
}

TEST(AddEntClusTest, HandlesFatImg) {
    unsigned char ramFDD144[BLOCKSIZE * 19];
    size_t cnt = loadFile("test/disk/fat.img", ramFDD144);
    ASSERT_EQ(cnt, BLOCKSIZE * 19)
        << "Can't find the disk file or file size doesn't match";
    int res = initFat(ramFDD144);
    ASSERT_EQ(res, 0) << "Init error";

    EXPECT_EQ(getNextClus(0x81f), 0x000);
    addEntClus(0x81f, 0x81f);
    EXPECT_EQ(getNextClus(0x81f), 0xfff);
    EXPECT_EQ(getFreeClus(), 0x820);
    addEntClus(0x81f, 0x820);
    EXPECT_EQ(getNextClus(0x81f), 0x820);
    EXPECT_EQ(getNextClus(0x820), 0xfff);
}

TEST(ClearClusTest, HandlesIOSYS) {
    unsigned char ramFDD144[BLOCKSIZE * 19];
    size_t cnt = loadFile("test/disk/fat.img", ramFDD144);
    ASSERT_EQ(cnt, BLOCKSIZE * 19)
        << "Can't find the disk file or file size doesn't match";
    int res = initFat(ramFDD144);
    ASSERT_EQ(res, 0) << "Init error";

    clearClus(0x002);
    EXPECT_EQ(getNextClus(0x000), 0xff0);
    EXPECT_EQ(getNextClus(0x001), 0xfff);
    for (unsigned short clus = 0x002; clus < 0x052; clus++)
        EXPECT_EQ(getNextClus(clus), 0);
    for (unsigned short clus = 0x052; clus < 0x81f; clus++)
        if (clus == 0x09d - 1 || clus == 0x108 - 1 || clus == 0x18a - 1 ||
            clus == 0x1a0 - 1 || clus == 0x1b8 - 1 || clus == 0x1d7 - 1 ||
            clus == 0x1f7 - 1 || clus == 0x231 - 1 || clus == 0x25e - 1 ||
            clus == 0x29e - 1 || clus == 0x2b1 - 1 || clus == 0x2b2 - 1 ||
            clus == 0x42e - 1 || clus == 0x521 - 1 || clus == 0x543 - 1 ||
            clus == 0x6a7 - 1 || clus == 0x7eb - 1 || clus == 0x81f - 1)
            EXPECT_EQ(getNextClus(clus), 0xfff);
        else
            EXPECT_EQ(getNextClus(clus), clus + 1);
    for (unsigned short clus = 0x81f; clus < BLOCKSIZE - 31; clus++)
        EXPECT_EQ(getNextClus(clus), 0);
}
