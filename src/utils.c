#include "utils.h"

bool diskStrEq(const char *str, const char *diskStr, int size) {
    for (size_t offset = 0; offset < size; offset++) {
        if (offset < strlen(str) && diskStr[offset] != str[offset])
            return false;
        if (offset >= strlen(str) && diskStr[offset] != ' ') return false;
    }
    return true;
}

unsigned int parseNum(const unsigned char *str, size_t base, size_t len) {
    unsigned int num = 0;
    for (size_t offset = 0; offset < len; offset++)
        num |= str[base + offset] << (offset * 8);
    return num;
}

void parseStr(const unsigned char *block, size_t base, size_t len, char *str) {
    for (size_t offset = 0; offset < len; offset++)
        str[offset] = block[base + offset];
}

void parseEntNum(unsigned int entNum, unsigned char *entStr, size_t base,
                 size_t len) {
    for (size_t offset = 0; offset < len; offset++)
        entStr[base + offset] = entNum & 0xff, entNum >>= 8;
}

void parseEntCharStr(const char *entCharStr, unsigned char *entStr, size_t base,
                     size_t len) {
    for (size_t offset = 0; offset < len; offset++)
        entStr[base + offset] = entCharStr[offset];
}

void printBlock(const unsigned char *block) {
    const int ENTPERLINE = 32;
    for (size_t i = 0; i < BLOCKSIZE; i++) {
        if (i % ENTPERLINE == 0)
            printf("%02X", block[i]);
        else
            printf(" %02X", block[i]);
        if (i % 8 == 7) printf(" ");
        if (i % ENTPERLINE == ENTPERLINE - 1) printf("\n");
    }
}

void printStr(const char *str, int len) {
    for (size_t offset = 0; offset < len; offset++) printf("%c", str[offset]);
}
