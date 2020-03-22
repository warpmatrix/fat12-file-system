#ifndef STRUCTRUE_H
#define STRUCTRUE_H

#include <stddef.h>

extern const int BLOCKNUM;
extern const int BLOCKSIZE;
extern const int SIZE;  // 2880 * 512

unsigned int parseNum(const unsigned char *block, size_t base, size_t len);
void parseStr(const unsigned char *block, size_t base, size_t len, char *str);

#endif
