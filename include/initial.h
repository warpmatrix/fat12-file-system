#ifndef INITIAL_H
#define INITIAL_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "fat12-header.h"

const int BLOCKNUM;
const int BLOCKSIZE;
const int SIZE;

bool init(unsigned char *ramFDD144);

int Read_ramFDD(unsigned char *ramFDD144, const char *filename);
void Read_ramFDD_Block(const unsigned char *ramFDD144, int blockIdx,
                       unsigned char *block);
void Write_ramFDD_Block(unsigned char *ramFDD144, int blockIdx,
                        const unsigned char *block);

unsigned int parseNum(const unsigned char *block, size_t base, size_t len);
void parseStr(const unsigned char *block, size_t base, size_t len, char *str);

void printBlock(const unsigned char *block);

#endif
