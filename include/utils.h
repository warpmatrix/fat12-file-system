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

void Read_ramFDD_Block(const unsigned char *ramFDD144, int blockIdx, unsigned char *block);
void Write_ramFDD_Block(unsigned char *ramFDD144, int blockIdx, const unsigned char *block);

unsigned short getNextClus(const unsigned char *ramFDD144, unsigned short clus);
unsigned short getFatClus(const unsigned char *fat, unsigned short clus);

unsigned int parseNum(const unsigned char *block, size_t base, size_t len);
void parseStr(const unsigned char *block, size_t base, size_t len, char *str);

void printStr(const char *str, int len);
void printBlock(const unsigned char *block);

#endif
