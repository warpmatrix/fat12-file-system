#include "utils.h"

#include <gtest/gtest.h>

TEST(ReadRamFDDTest, HandlesDisk1) {
    EXPECT_EQ(SIZE, 2880 * 512);
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/disk1.flp");
    EXPECT_EQ(res, SIZE);
    FILE *fp = fopen("test/disk/disk1.flp", "rb");
    for (size_t offset = 0; offset < SIZE; offset++) {
        unsigned char ch;
        fscanf(fp, "%c", &ch);
        EXPECT_EQ(ramFDD144[offset], ch);
    }
    fclose(fp);
}

TEST(ReadRamFDDBlockTest, HandlesBlock0) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/disk1.flp");
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 0, block);
    for (size_t offset = 0; offset < BLOCKSIZE; offset++)
        EXPECT_EQ(block[offset], ramFDD144[offset]);
}

TEST(ReadRamFDDBlockTest, HandlesBlock1) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/disk1.flp");
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 1, block);
    for (size_t offset = 0; offset < BLOCKSIZE; offset++)
        EXPECT_EQ(block[offset], ramFDD144[offset + BLOCKSIZE]);
}

TEST(WriteRamFDDBlockTest, HandlesBlock2) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/disk1.flp");
    unsigned char RamFDD144Copy[SIZE];
    for (size_t offset = 0; offset < SIZE; offset++)
        RamFDD144Copy[offset] = ramFDD144[offset];
    unsigned char block[BLOCKSIZE];
    Write_ramFDD_Block(block, ramFDD144, 2);
    for (size_t baseSec = 0; baseSec < BLOCKNUM; baseSec++)
        if (baseSec == 2) {
            for (size_t offset = 0; offset < BLOCKSIZE; offset++)
                EXPECT_EQ(ramFDD144[baseSec * BLOCKSIZE + offset],
                          block[offset]);
        } else
            for (size_t offset = 0; offset < BLOCKSIZE; offset++)
                EXPECT_EQ(ramFDD144[baseSec * BLOCKSIZE + offset],
                          RamFDD144Copy[baseSec * BLOCKSIZE + offset]);
}

TEST(ParseNumTest, HandlesNumStr) {
    const unsigned char testStr[] = "\x32\xf0";
    EXPECT_EQ(parseNum(testStr, 0, 2), 0xf032);
}

TEST(ParseNumTest, HandlesStrWithLetter) {
    const unsigned char testStr[] = "I \x32\xf0\xbf\x17";
    EXPECT_EQ(parseNum(testStr, 2, 4), 0x17bff032);
}

TEST(ParseStrTest, HandlesExampleStr) {
    unsigned char block[] = "\xeb\x3c\x90MSDOS5.0\x02\x01";
    char str[8];
    parseStr(block, 3, 8, str);
    for (size_t offset = 0; offset < 8; offset++)
        EXPECT_EQ(str[offset], "MSDOS5.0"[offset]);
}

TEST(DiskStrTest, HandlesStrUsers) {
    char diskStr[11] = "USERS     ";
    diskStr[10] = ' ';
    EXPECT_TRUE(diskStrEq("users", diskStr, 11));
}

TEST(DiskStrTest, HandlesStrDot) {
    char diskStr[11] = ".         ";
    diskStr[10] = ' ';
    EXPECT_TRUE(diskStrEq(".", diskStr, 11));
}

TEST(DiskStrTest, HandlesStrDdot) {
    char diskStr[11] = "..        ";
    diskStr[10] = ' ';
    EXPECT_TRUE(diskStrEq("..", diskStr, 11));
}
