#include <gtest/gtest.h>

extern "C" {
#include "io.h"
}

TEST(ReadRamFDDTest, HandlesDisk1) {
    EXPECT_EQ(SIZE, 2880 * 512);
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/startup.flp");
    EXPECT_EQ(res, SIZE);
    FILE *fp = fopen("test/disk/startup.flp", "rb");
    for (size_t offset = 0; offset < SIZE; offset++) {
        unsigned char ch;
        fscanf(fp, "%c", &ch);
        EXPECT_EQ(ramFDD144[offset], ch);
    }
    fclose(fp);
}

TEST(ReadRamFDDBlockTest, HandlesBlock0) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/startup.flp");
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 0, block);
    for (size_t offset = 0; offset < BLOCKSIZE; offset++)
        EXPECT_EQ(block[offset], ramFDD144[offset]);
}

TEST(ReadRamFDDBlockTest, HandlesBlock1) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/startup.flp");
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 1, block);
    for (size_t offset = 0; offset < BLOCKSIZE; offset++)
        EXPECT_EQ(block[offset], ramFDD144[offset + BLOCKSIZE]);
}

TEST(WriteRamFDDBlockTest, HandlesBlock2) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/startup.flp");
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
