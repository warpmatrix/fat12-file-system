#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

#define BLOCKNUM 2880
#define BLOCKSIZE 512
#define SIZE (BLOCKNUM*BLOCKSIZE)
extern const char FILENAME[];

unsigned int parseNum(const unsigned char *str, size_t base, size_t len);
void parseEntNum(unsigned int entNum, unsigned char *entStr, size_t base, size_t len);

void parseStr(const unsigned char *block, size_t base, size_t len, char *str);
void parseEntCharStr(const char *entCharStr, unsigned char *entStr, size_t base, size_t len);

bool diskStrEq(const char *str, const char *diskStr, int size);

void printStr(const char *str, int len);
void printBlock(const unsigned char *block);

#endif
