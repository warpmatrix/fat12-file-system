#include "test-utils.hpp"

void MY_EXPECT_STREQ(const char *diskStr, const char *str, int size) {
    for (size_t offset = 0; offset < size; offset++) {
        if (offset < strlen(str))
            EXPECT_EQ(diskStr[offset], str[offset]);
        else
            EXPECT_EQ(diskStr[offset], ' ');
    }
}
