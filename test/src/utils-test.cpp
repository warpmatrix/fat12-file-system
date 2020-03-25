#include "utils.h"

#include <gtest/gtest.h>

TEST(ParseNumTest, HandlesNumStr) {
    const unsigned char testStr[] = "\x32\xf0";
    EXPECT_EQ(parseNum(testStr, 0, 2), 0xf032);
}

TEST(ParseNumTest, HandlesComplxStr) {
    const unsigned char testStr[] = "I \x32\xf0\xbf\x17";
    EXPECT_EQ(parseNum(testStr, 2, 4), 0x17bff032);
}
