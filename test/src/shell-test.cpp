#include <gtest/gtest.h>

extern "C" {
#include "shell.h"
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
