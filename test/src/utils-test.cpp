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
    EXPECT_FALSE(diskStrEq(".users", diskStr, 11));
    EXPECT_FALSE(diskStrEq("users.", diskStr, 11));
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
