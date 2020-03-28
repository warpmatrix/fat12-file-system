#include <gtest/gtest.h>
#include "shell.h"

TEST(ParseInpTest, HandlesOneWordStr) {
    Command cmd;
    char input[] = "quit\n";
    parseInp(input, &cmd);
    EXPECT_EQ(cmd.argc, 1);
    EXPECT_STREQ(cmd.argv[0], "quit");
    freeCmd(&cmd);
}

TEST(ParseInpTest, HandlesMoreWordsStr) {
    Command cmd;
    char input[] = "dir ./../abc\n";
    parseInp(input, &cmd);
    EXPECT_EQ(cmd.argc, 2);
    EXPECT_STREQ(cmd.argv[0], "dir");
    EXPECT_STREQ(cmd.argv[1], "./../abc");
    freeCmd(&cmd);
}

TEST(ParseInpTest, HandlesEmptyStr) {
    Command cmd;
    char input[] = "\n";
    parseInp(input, &cmd);
    EXPECT_EQ(cmd.argc, 0);
    freeCmd(&cmd);
}
