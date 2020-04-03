#ifndef FAT_H
#define FAT_H

#include <stddef.h>

#include "utils.h"
#include "entry.h"

int initFat(const unsigned char *ramFDD144);
void writeFats(unsigned char *ramFDD144);

unsigned short getNextClus(unsigned short clus);
unsigned short getFreeClus();

void addEntClus(unsigned short fstClus, unsigned short newClus);
void setFatClus(unsigned short clus, unsigned short newClus);

const unsigned char *getFat();

#endif
