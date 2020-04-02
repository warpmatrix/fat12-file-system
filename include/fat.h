#ifndef FAT_H
#define FAT_H

#include <stddef.h>

#include "utils.h"
#include "entry.h"

unsigned short getNextClus(const unsigned char *ramFDD144, unsigned short clus);
unsigned short getFatClus(const unsigned char *fat, unsigned short clus);
unsigned short getFreeClus(const unsigned char *ramFDD144);

void addEntClus(unsigned char *ramFDD144, unsigned short fstClus, unsigned short newClus);
void setFatClus(unsigned char *fat, unsigned short clus, unsigned short newClus);

#endif
