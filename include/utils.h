#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "fat12-header.h"
#include "entry.h"

extern const int BLOCKNUM;
extern const int BLOCKSIZE;
extern const int SIZE;
extern const char FILENAME[];

// fail to find the file , return -1;
// otherwise, return the bytes of the file.
int Read_ramFDD(unsigned char *ramFDD144, const char *filename);
void Read_ramFDD_Block(const unsigned char *ramFDD144, int blockIdx,
                       unsigned char *block);
void Write_ramFDD_Block(const unsigned char *block, unsigned char *ramFDD144,
                        int blockIdx);

unsigned short getNextClus(const unsigned char *ramFDD144, unsigned short clus);
unsigned short getFatClus(const unsigned char *fat, unsigned short clus);
unsigned short getEntClus(unsigned short fstClus, const char *entname, const unsigned char *ramFDD144);

unsigned short parsePath(const char *path, unsigned short clus,
                         const unsigned char *ramFDD144);
unsigned int parseNum(const unsigned char *str, size_t base, size_t len);
void parseStr(const unsigned char *block, size_t base, size_t len, char *str);

bool diskStrEq(const char *str, const char *diskStr, int size);

void printStr(const char *str, int len);
void printBlock(const unsigned char *block);

#endif
