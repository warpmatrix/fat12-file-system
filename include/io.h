#ifndef IO_H
#define IO_H

#include <stddef.h>
#include <stdio.h>

#include "utils.h"

// fail to find the file , return -1;
// otherwise, return the bytes of the file.
int Read_ramFDD(unsigned char *ramFDD144, const char *diskname);
int Write_ramFDD(const unsigned char *ramFDD144, const char *diskname);

void Read_ramFDD_Block(const unsigned char *ramFDD144, int blockIdx,
                       unsigned char *block);
void Write_ramFDD_Block(const unsigned char *block, unsigned char *ramFDD144,
                        int blockIdx);

#endif
