#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include "structrue.h"
#include "fat12-header.h"
#include "root-entry.h"

void printBlock(const unsigned char *block);
void printMbrInfo(Fat12Header mbr);

#endif
