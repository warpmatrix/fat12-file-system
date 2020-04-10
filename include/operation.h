#if !defined(OPERATION_H)
#define OPERATION_H

#include "dir-entry.h"
#include "entry.h"

#define MISS_OPERAND 1
#define NO_FILE_DIR 2
#define FILE_DIR_XST 3
#define ROOT_IS_FULL 4
#define DISK_IS_FULL 5
#define NOT_RM_PRES_DIR 6
#define INVALID_ARGUMET 7
#define IS_DIRECTORY 8
#define DIR_NOT_EMPTY 9
#define FILE_ERROR 10
#define OPER_NOT_PERM 11
#define MISS_DEST_FILE 12

#define CMD_NOT_FOUND -1
#define DIR_HAS_DOT -2
#define SOURCE_IS_DIR -3

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
