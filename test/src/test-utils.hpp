#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <gtest/gtest.h>
#include <string.h>

void MY_EXPECT_STREQ(const char *diskStr, const char *str, int size);
size_t loadFile(const char *filename, unsigned char *block);

#endif
