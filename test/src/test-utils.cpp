#include "test-utils.hpp"

void MY_EXPECT_STREQ(const char *diskStr, const char *str, int size) {
    for (size_t offset = 0; offset < size; offset++)
        if (offset < strlen(str))
            EXPECT_EQ(diskStr[offset], str[offset]);
        else
            EXPECT_EQ(diskStr[offset], ' ');
}

size_t loadFile(const char *filename, unsigned char *block) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) return -1;
    size_t cnt = 0;
    while (fscanf(fp, "%c", &block[cnt]) != EOF) cnt++;
    fclose(fp);
    return cnt;
}
