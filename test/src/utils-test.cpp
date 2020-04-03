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

TEST(ParseStrTest, HandlesExampleStr) {
    unsigned char block[] = "\xeb\x3c\x90MSDOS5.0\x02\x01";
    char str[8];
    parseStr(block, 3, 8, str);
    for (size_t offset = 0; offset < 8; offset++)
        EXPECT_EQ(str[offset], "MSDOS5.0"[offset]);
}

TEST(ParseEntCharStrTest, HandlesEntChatStr) {
    char entCharStr[11] = "user      ";
    entCharStr[10] = ' ';
    unsigned char entStr[13];
    parseEntCharStr(entCharStr, entStr, 1, 11);
}

TEST(DiskStrEqTest, HandlesStrUsers) {
    char diskStr[11] = "USERS     ";
    diskStr[10] = ' ';
    EXPECT_TRUE(diskStrEq("USERS", diskStr, 11));
    EXPECT_FALSE(diskStrEq("users", diskStr, 11));
    EXPECT_FALSE(diskStrEq(".USERS", diskStr, 11));
    EXPECT_FALSE(diskStrEq("USERS.", diskStr, 11));
}

TEST(DiskStrEqTest, HandlesStrDot) {
    char diskStr[11] = ".         ";
    diskStr[10] = ' ';
    EXPECT_TRUE(diskStrEq(".", diskStr, 11));
    EXPECT_FALSE(diskStrEq("..", diskStr, 11));
}

TEST(DiskStrEqTest, HandlesStrDdot) {
    char diskStr[11] = "..        ";
    diskStr[10] = ' ';
    EXPECT_TRUE(diskStrEq("..", diskStr, 11));
    EXPECT_FALSE(diskStrEq(".", diskStr, 11));
}
