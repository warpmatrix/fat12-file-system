#if !defined(OPERATION_H)
#define OPERATION_H

#include "dir-entry.h"
#include "entry.h"

int dispFile(const Entry *fileEnt, const unsigned char *ramFDD144);
int editFile(Entry *fileEnt, unsigned short dirClus, unsigned char *ramFDD144);

int mkent(const char *entname, unsigned short dirClus,
          unsigned char *ramFDD144);
int mkdirent(const char *entname, unsigned short dirClus,
             unsigned char *ramFDD144);

int cpent(const Entry *entry, const char *destEntname,
          unsigned short destDirClus, unsigned char *ramFDD144);

bool markEntDel(unsigned short entClus, unsigned char *block);
int rment(unsigned short entClus, unsigned short dirClus,
          unsigned char *ramFDD144);

void printEnts(const Entry *entries, int entCnt);
void listEnts(unsigned short fstClus, const unsigned char *ramFDD144);
void printTree(unsigned short dirClus, Stack *lastEnt,
               const unsigned char *ramFDD144);

#endif  // OPERATION_H
