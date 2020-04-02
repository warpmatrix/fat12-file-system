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

TEST(ParseEntTest, HandlesDotDirEnt) {
    Entry entry;
    strcpy(entry.DIR_Name, ".         "), entry.DIR_Name[10] = ' ';
    entry.DIR_Attr = 0x10;
    memset(entry.Reserve, 0, 10);
    entry.DIR_WrtTime = 0x4AAE;
    entry.DIR_WrtDate = 0x4936;
    entry.DIR_FstClus = 0;
    entry.DIR_FileSize = 0;
    unsigned char entStr[32];
    parseEnt(&entry, entStr);
    unsigned char corrEntStr[] =
        ".          "
        "\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xAE\x4A\x36\x49\x00\x00"
        "\x00\x00\x00\x00";
    for (size_t offset = 0; offset < 32; offset++)
        EXPECT_EQ(entStr[offset], corrEntStr[offset]) << offset;
}

TEST(ParseEntsTest, HandlesEmptyDirEntry) {
    unsigned short newEntClus = 2, dirClus = 0;
    Entry entries[2];
    time_t wrtTime = 1585789263;
    entries[0] = mknewEnt(".", 0x10, wrtTime, newEntClus, 0);
    entries[1] = mknewEnt("..", 0x10, wrtTime, dirClus, 0);
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

TEST(ParseEntStrTest, HandlesExampleEntry) {
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
    EXPECT_TRUE(entnameEq("USERS", entname));
    EXPECT_FALSE(entnameEq("users", entname));
    EXPECT_FALSE(entnameEq(".USERS", entname));
    EXPECT_TRUE(entnameEq("USERS.", entname));
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
    EXPECT_TRUE(entnameEq("A.TXT", entname));
    EXPECT_FALSE(entnameEq("a.txt", entname));
    EXPECT_FALSE(entnameEq("ATXT", entname));
    EXPECT_FALSE(entnameEq(".A.TXT", entname));
    EXPECT_FALSE(entnameEq("A..TXT", entname));
    EXPECT_FALSE(entnameEq(".ATXT", entname));
}

TEST(EntnameEqTest, HandlesStrWithoutMainname) {
    char entname[11] = "        TX";
    entname[10] = 'T';
    EXPECT_TRUE(entnameEq(".TXT", entname));
    EXPECT_FALSE(entnameEq(".txt", entname));
    EXPECT_FALSE(entnameEq("TXT", entname));
    EXPECT_FALSE(entnameEq("..TXT", entname));
}

TEST(ParseTimeTest, HandlesExampleTime) {
    unsigned short wrtTime, wrtDate;
    time_t timer = 1585789263;
    parseTime(timer, &wrtTime, &wrtDate);
    EXPECT_EQ(wrtTime, 18465);  // 09:01:02
    EXPECT_EQ(wrtDate, 20610);  // 2020-04-02
}
