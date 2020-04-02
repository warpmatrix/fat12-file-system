#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define BLOCKNUM 2880
extern const int BLOCKSIZE;
extern const int SIZE;
extern const char FILENAME[];

#include "entry.h"
#include "fat12-header.h"


unsigned int parseNum(const unsigned char *str, size_t base, size_t len);
void parseStr(const unsigned char *block, size_t base, size_t len, char *str);

bool diskStrEq(const char *str, const char *diskStr, int size);

void printStr(const char *str, int len);
void printBlock(const unsigned char *block);

// ret entClus and modify dirClus
unsigned short parsePath(unsigned short *dirClus, const char *path,
                         const unsigned char *ramFDD144);
size_t findPath(char **path, unsigned short entClus,
                const unsigned char *ramFDD144);
void printPath(unsigned short clus, const unsigned char *ramFDD144);

#endif
