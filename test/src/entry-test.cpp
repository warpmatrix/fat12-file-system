#include <gtest/gtest.h>
#include <string.h>

extern "C" {
#include "entry.h"
}


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
    unsigned char entStr[] =
        "IO      SYS\x07          \xC0\x32\xBF\x1C\x02\x00\x46\x9F\x00\x00";
    Entry entry = parseEntStr(entStr);
    char filename[] = "IO      SYS";
    for (size_t i = 0; i < 11; i++) EXPECT_EQ(entry.DIR_Name[i], filename[i]);
    EXPECT_EQ(entry.DIR_Attr, 0x07);
    EXPECT_EQ(entry.DIR_WrtTime, 0x32C0);
    EXPECT_EQ(entry.DIR_WrtDate, 0x1CBF);
    EXPECT_EQ(entry.DIR_FstClus, 0x0002);
    EXPECT_EQ(entry.DIR_FileSize, 0x00009F46);
}

TEST(EntnameEqTest, HandlesStrUsers) {
    char entname[11] = "USERS     ";
    entname[10] = ' ';
    EXPECT_TRUE(entnameEq("users", entname));
    EXPECT_FALSE(entnameEq(".users", entname));
    EXPECT_TRUE(entnameEq("users.", entname));
    // EXPECT_FALSE(entnameEq("users.", entname));
}

TEST(EntnameEqTest, HandlesStrDot) {
    char entname[11] = ".         ";
    entname[10] = ' ';
    EXPECT_TRUE(entnameEq(".", entname));
}

TEST(EntnameEqTest, HandlesStrDdot) {
    char entname[11] = "..        ";
    entname[10] = ' ';
    EXPECT_TRUE(entnameEq("..", entname));
}

TEST(EntnameEqTest, HandlesStrWithExtname) {
    char entname[11] = "A       TX";
    entname[10] = 'T';
    EXPECT_TRUE(entnameEq("a.txt", entname));
    EXPECT_FALSE(entnameEq("atxt", entname));
    EXPECT_FALSE(entnameEq(".a.txt", entname));
    EXPECT_FALSE(entnameEq("a..txt", entname));
    EXPECT_FALSE(entnameEq(".atxt", entname));
}

TEST(EntnameEqTest, HandlesStrWithoutMainname) {
    char entname[11] = "        TX";
    entname[10] = 'T';
    EXPECT_TRUE(entnameEq(".txt", entname));
    EXPECT_FALSE(entnameEq("txt", entname));
    EXPECT_FALSE(entnameEq("..txt", entname));
}

TEST(ParseTimeTest, HandlesExampleTime) {
    unsigned short wrtTime, wrtDate;
    time_t timer = 1585789263;
    parseTime(timer, &wrtTime, &wrtDate);
    char time[20];
    parseWriTime(wrtTime, wrtDate, time);
    EXPECT_STREQ(time, "2020-04-02 09:01:02");
}
