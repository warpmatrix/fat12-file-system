#include <gtest/gtest.h>
#include <string.h>
#include "test-utils.hpp"

extern "C" {
#include "dir-entry.h"
}

TEST(ParseEntsTest, HandlesEmptyDirEntry) {
    unsigned short newEntClus = 2, dirClus = 0;
    Entry entries[2];
    time_t wrtTime = 1585789263;
    entries[0] = getEnt(".", DIR_ATTR, wrtTime, newEntClus, 0);
    entries[1] = getEnt("..", DIR_ATTR, wrtTime, dirClus, 0);
    unsigned char block[BLOCKSIZE];
    parseEnts(entries, 2, block);
    unsigned char entStr1[] =
        ".          "
        "\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x21\x48\x82\x50\x02\x00"
        "\x00\x00\x00\x00";
    unsigned char entStr2[] =
        "..         "
        "\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x21\x48\x82\x50\x00\x00"
        "\x00\x00\x00\x00";
    for (size_t offset = 0; offset < 32; offset++)
        EXPECT_EQ(block[offset], entStr1[offset]) << offset;
    for (size_t offset = 0; offset < 32; offset++)
        EXPECT_EQ(block[32 + offset], entStr2[offset]);
    for (size_t offset = 64; offset < BLOCKSIZE; offset++)
        EXPECT_EQ(block[offset], 0x0);
}

TEST(ParseEntBlock, HandlesRootEnts) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/startup.flp");
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 20, block);
    Entry entries[16];
    size_t entCnt = parseEntBlock(block, entries);
    EXPECT_EQ(entCnt, 4);
    EXPECT_EQ(entries[0].DIR_FstClus, 0x521);
    EXPECT_EQ(entries[1].DIR_FstClus, 0x543);
    EXPECT_EQ(entries[2].DIR_FstClus, 0x6a7);
    EXPECT_EQ(entries[3].DIR_FstClus, 0x7eb);
}

TEST(GetEntByNameTest, HandlesRootEntAndSubdirEnt) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/test-disk.flp");
    Entry entry = getEntByName("IO.SYS", 0, ramFDD144);
    EXPECT_EQ(entry.DIR_FstClus, 0x002);
    entry = getEntByName("MATRIX", 0x81f, ramFDD144);
    EXPECT_EQ(entry.DIR_FstClus, 0x821);
}

TEST(GetEntByNameTest, HandlesNullEnt) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/test-disk.flp");
    Entry entry = getEntByName("null.ent", 0, ramFDD144);
    EXPECT_EQ(entry.DIR_FstClus, (unsigned short)-1);
}

TEST(GetEntByClusTest, HandlesRootEntAndSubdirEnt) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/test-disk.flp");
    Entry entry = getEntByClus(0x81f, 0, ramFDD144);
    MY_EXPECT_STREQ(entry.DIR_Name, "USER", 11);
    EXPECT_EQ(entry.DIR_FstClus, 0x81f);
    entry = getEntByClus(0x821, 0x81f, ramFDD144);
    MY_EXPECT_STREQ(entry.DIR_Name, "MATRIX", 11);
    EXPECT_EQ(entry.DIR_FstClus, 0x821);
}
