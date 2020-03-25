#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "fat12-header.h"
#include "entry.h"

extern const int BLOCKNUM;
extern const int BLOCKSIZE;
extern const int SIZE;
extern const char FILENAME[];

int Read_ramFDD(unsigned char *ramFDD144, const char *filename);
void Read_ramFDD_Block(const unsigned char *ramFDD144, int blockIdx, unsigned char *block);
void Write_ramFDD_Block(const unsigned char *block, unsigned char *ramFDD144, int blockIdx);

unsigned short getNextClus(const unsigned char *ramFDD144, unsigned short clus);
unsigned short getFatClus(const unsigned char *fat, unsigned short clus);

bool diskStrEq(const char *diskStr, const char *str, int size);
unsigned int parseNum(const unsigned char *str, size_t base, size_t len);
void parseStr(const unsigned char *block, size_t base, size_t len, char *str);

void printStr(const char *str, int len);
void printBlock(const unsigned char *block);

#endif
