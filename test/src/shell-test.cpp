#include <gtest/gtest.h>

extern "C" {
#include "shell.h"
}

TEST(InitCmdTest, HandlesExampleCmd) {
    Command cmd;
    initCmd(&cmd);
    EXPECT_EQ(cmd.argc, 0);
    for (size_t i = 0; i < CMDLEN / 2 + 1; i++) EXPECT_STREQ(cmd.argv[i], NULL);
}

TEST(ParseInpTest, HandlesOneWordStr) {
    char input[] = "quit\n";
    Command cmd = parseInp(input);
    EXPECT_EQ(cmd.argc, 1);
    EXPECT_STREQ(cmd.argv[0], "quit");
    freeCmd(&cmd);
}

TEST(ParseInpTest, HandlesMoreWordsStr) {
    char input[] = "dir ./../abc\n";
    Command cmd = parseInp(input);
    EXPECT_EQ(cmd.argc, 2);
    EXPECT_STREQ(cmd.argv[0], "dir");
    EXPECT_STREQ(cmd.argv[1], "./../abc");
    freeCmd(&cmd);
}

TEST(ParseInpTest, HandlesEmptyStr) {
    char input[] = "\n";
    Command cmd = parseInp(input);
    EXPECT_EQ(cmd.argc, 0);
    freeCmd(&cmd);
}

TEST(ParsePathTest, HandlesRootEntry) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/startup.flp");
    ASSERT_EQ(res, SIZE);
    unsigned short dirClus = 0;
    unsigned short entClus = parsePath(&dirClus, "IO.SYS", ramFDD144);
    EXPECT_EQ(entClus, 0x002);
    EXPECT_EQ(dirClus, 0x000);
}

TEST(ParsePathTest, HandlesSubDirEntry) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/test-disk.flp");
    ASSERT_EQ(res, SIZE);
    res = initFat(ramFDD144);
    EXPECT_EQ(res, 0);

    unsigned short dirClus = 0;
    unsigned short entClus = parsePath(&dirClus, "USER", ramFDD144);
    EXPECT_EQ(entClus, 0x81f);
    EXPECT_EQ(dirClus, 0x000);

    dirClus = entClus;
    entClus = parsePath(&dirClus, "MATRIX/MUSIC/", ramFDD144);
    EXPECT_EQ(entClus, 0x822);
    EXPECT_EQ(dirClus, 0x821);

    dirClus = entClus;
    entClus = parsePath(&dirClus, "../../../COMMAND.COM", ramFDD144);
    EXPECT_EQ(entClus, 0x09d);
    EXPECT_EQ(dirClus, 0x0);

    dirClus = entClus;
    entClus = parsePath(&dirClus, "/", ramFDD144);
    EXPECT_EQ(entClus, 0);
    EXPECT_EQ(dirClus, 0);

    dirClus = 0;
    entClus = parsePath(&dirClus, "USER/", ramFDD144);
    EXPECT_EQ(entClus, 0x81f);
    EXPECT_EQ(dirClus, 0x000);
}

TEST(ParsePathTest, HandlesNullEntry) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/test-disk.flp");
    ASSERT_EQ(res, SIZE);
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
    entClus = parsePath(&dirClus, "USER//", ramFDD144);
    EXPECT_EQ(entClus, (unsigned short)-1);
    EXPECT_EQ(dirClus, 0x81f);

    dirClus = 0;
    entClus = parsePath(&dirClus, "null/null.ent", ramFDD144);
    EXPECT_EQ(entClus, (unsigned short)-1);
    EXPECT_EQ(dirClus, (unsigned short)-1);
}

TEST(FindPathTest, HandlesUserMatrixMusic) {
    unsigned char ramFDD144[SIZE];
    int res = Read_ramFDD(ramFDD144, "test/disk/test-disk.flp");
    ASSERT_EQ(res, SIZE);
    char path[BLOCKNUM - 33][12];
    size_t cnt = findPath(path, 0x81f, ramFDD144);
    EXPECT_EQ(cnt, 2);
    EXPECT_STREQ(path[1], "/");
    EXPECT_STREQ(path[0], "USER");
    cnt = findPath(path, 0x822, ramFDD144);
    EXPECT_EQ(cnt, 4);
    EXPECT_STREQ(path[3], "/");
    EXPECT_STREQ(path[2], "USER");
    EXPECT_STREQ(path[1], "MATRIX");
    EXPECT_STREQ(path[0], "MUSIC");
}
