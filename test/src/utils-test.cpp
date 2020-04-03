#include <gtest/gtest.h>

extern "C" {
#include "utils.h"
}

TEST(ParseNumTest, HandlesNumStr) {
    const unsigned char testStr[] = "\x32\xf0";
    EXPECT_EQ(parseNum(testStr, 0, 2), 0xf032);
}

TEST(ParseNumTest, HandlesStrWithLetter) {
    const unsigned char testStr[] = "I \x32\xf0\xbf\x17";
    EXPECT_EQ(parseNum(testStr, 2, 4), 0x17bff032);
}

TEST(ParseEntNumTest, HandlesZero) {
    unsigned char entStr[6];
    parseEntNum(0, entStr, 1, 4);
    for (size_t i = 1; i < 5; i++) EXPECT_EQ(entStr[i], 0);
}

TEST(ParseEntNumTest, HandlesPositiveNumber) {
    unsigned char entStr[6];
    parseEntNum(0x1032f102, entStr, 1, 4);
    EXPECT_EQ(entStr[1], 0x02);
    EXPECT_EQ(entStr[2], 0xf1);
    EXPECT_EQ(entStr[3], 0x32);
    EXPECT_EQ(entStr[4], 0x10);
}

TEST(ParseEntCharStrTest, HandlesEntChatStr) {
    char entCharStr[11] = "user      ";
    entCharStr[10] = ' ';
    unsigned char entStr[13];
    parseEntCharStr(entCharStr, entStr, 1, 11);
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
    EXPECT_TRUE(diskStrEq("USERS", diskStr, 11));
    EXPECT_FALSE(diskStrEq("users", diskStr, 11));
    EXPECT_FALSE(diskStrEq(".USERS", diskStr, 11));
    EXPECT_FALSE(diskStrEq("USERS.", diskStr, 11));
}

TEST(DiskStrTest, HandlesStrDot) {
    char diskStr[11] = ".         ";
    diskStr[10] = ' ';
    EXPECT_TRUE(diskStrEq(".", diskStr, 11));
    EXPECT_FALSE(diskStrEq("..", diskStr, 11));
}

TEST(DiskStrTest, HandlesStrDdot) {
    char diskStr[11] = "..        ";
    diskStr[10] = ' ';
    EXPECT_TRUE(diskStrEq("..", diskStr, 11));
    EXPECT_FALSE(diskStrEq(".", diskStr, 11));
}

TEST(ParsePathTest, HandlesRootEntry) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/startup.flp");
    EXPECT_EQ(res, SIZE);
    unsigned short dirClus = 0;
    unsigned short entClus = parsePath(&dirClus, "IO.SYS", ramFDD144);
    EXPECT_EQ(entClus, 0x002);
    EXPECT_EQ(dirClus, 0x000);
}

TEST(ParsePathTest, HandlesSubDirEntry) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/test-disk.flp");
    EXPECT_EQ(res, SIZE);
    res = initFat(ramFDD144);
    EXPECT_EQ(res, 0);

    unsigned short dirClus = 0;
    unsigned short entClus = parsePath(&dirClus, "USER", ramFDD144);
    EXPECT_EQ(entClus, 0x81f);
    EXPECT_EQ(dirClus, 0x000);

    dirClus = entClus;
    entClus = parsePath(&dirClus, "MATRIX/MUSIC", ramFDD144);
    EXPECT_EQ(entClus, 0x822);
    EXPECT_EQ(dirClus, 0x821);

    dirClus = entClus;
    entClus = parsePath(&dirClus, "../../../COMMAND.COM", ramFDD144);
    EXPECT_EQ(entClus, 0x09d);
    EXPECT_EQ(dirClus, 0x0);
}

TEST(ParsePathTest, HandlesNullEntry) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/test-disk.flp");
    EXPECT_EQ(res, SIZE);
    res = initFat(ramFDD144);
    EXPECT_EQ(res, 0);

    unsigned short dirClus = 0;
    unsigned short entClus = parsePath(&dirClus, "null.ent", ramFDD144);
    EXPECT_EQ(entClus, (unsigned short)-1);
    EXPECT_EQ(dirClus, 0x000);

    dirClus = 0;
    entClus = parsePath(&dirClus, "USER/null.ent", ramFDD144);
    EXPECT_EQ(entClus, (unsigned short)-1);
    EXPECT_EQ(dirClus, 0x81f);

    dirClus = 0;
    entClus = parsePath(&dirClus, "null/null.ent", ramFDD144);
    EXPECT_EQ(entClus, (unsigned short)-1);
    EXPECT_EQ(dirClus, (unsigned short)-1);
}
