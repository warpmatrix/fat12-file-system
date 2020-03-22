#include <gtest/gtest.h>
#include <string.h>
#include "entry.h"

TEST(DaysPerMonTest, HandlesFeb) {
    EXPECT_EQ(daysPerMon(2000, 2), 29);
    EXPECT_EQ(daysPerMon(2001, 2), 28);
}

TEST(DaysPerMonTest, HandlesOtherMonth) {
    EXPECT_EQ(daysPerMon(2000, 4), 30);
    EXPECT_EQ(daysPerMon(2001, 6), 30);
    EXPECT_EQ(daysPerMon(2002, 9), 30);
    EXPECT_EQ(daysPerMon(2003, 11), 30);
    EXPECT_EQ(daysPerMon(2004, 1), 31);
    EXPECT_EQ(daysPerMon(2005, 3), 31);
}

TEST(IsLeapTest, HandlesLeapOrNonLeapYear) {
    EXPECT_TRUE(isLeap(2000));
    EXPECT_TRUE(isLeap(2004));
    EXPECT_FALSE(isLeap(1900));
    EXPECT_FALSE(isLeap(2001));
}

TEST(ParseWriTimeTest, HandlesExampleTime) {
    char time[20];
    parseWriTime(0x4AAE, 0x4936, time);
    EXPECT_EQ(strlen(time), 19);
    EXPECT_STREQ(time, "2016-09-22 09:21:28");
}

TEST(ParseEntTest, HandlesExampleEntry) {
    unsigned char block[] =
        "\x49\x4F\x20\x20\x20\x20\x20\x20\x53\x59\x53\x07\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\xC0\x32\xBF\x1C\x02\x00\x46\x9F\x00\x00";
    Entry entry;
    parseEnt(block, &entry);
    char filename[] = "IO      SYS";
    for (size_t i = 0; i < 11; i++) EXPECT_EQ(entry.DIR_Name[i], filename[i]);
    EXPECT_EQ(entry.DIR_Attr, 0x07);
    EXPECT_EQ(entry.DIR_WrtTime, 0x32C0);
    EXPECT_EQ(entry.DIR_WrtDate, 0x1CBF);
    EXPECT_EQ(entry.DIR_FstClus, 0x0002);
    EXPECT_EQ(entry.DIR_FileSize, 0x00009F46);
}