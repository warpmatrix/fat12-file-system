#include "structrue.h"

const int BLOCKNUM = 2880;
const int BLOCKSIZE = 512;
const int SIZE = 1474560;  // 2880 * 512

unsigned int parseNum(const unsigned char *block, size_t base, size_t len) {
    unsigned int num = 0;
    for (size_t offset = 0; offset < len; offset++) {
        num += block[base + offset] * (1 << (offset * 8));
    }
    return num;
}

void parseStr(const unsigned char *block, size_t base, size_t len, char *str) {
    for (size_t offset = 0; offset < len; offset++)
        str[offset] = block[base + offset];
}
