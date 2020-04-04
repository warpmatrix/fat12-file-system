#include "command.h"

int lscmd(unsigned short curClus, const char *path,
          const unsigned char *ramFDD144) {
    unsigned short entClus = curClus, dirClus = curClus;
    if (path) entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return -1;
    if (entClus) {
        Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
        if (entry.DIR_Attr != 0x10) return -1;
    }
    listEnts(entClus, ramFDD144);
    return 0;
}

int cdcmd(unsigned short *clus, const char *path,
          const unsigned char *ramFDD144) {
    unsigned short entClus = 0, dirClus = *clus;
    if (path) entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return -1;
    if (entClus) {
        Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
        if (entry.DIR_Attr != 0x10) return -1;
    }
    *clus = entClus;
    return 0;
}

int mkdircmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return -1;  // missing operand
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (dirClus == (unsigned short)-1) return -2;  // dir doesn't exist
    if (entClus != (unsigned short)-1) return -3;  // target exist
    char delim[] = "/";
    char *pathCopy = strdup(path), *cpyPtr = pathCopy;
    const char *entname = strsep(&pathCopy, delim);
    while (pathCopy) entname = strsep(&pathCopy, delim);
    int res = mkdirent(entname, dirClus, ramFDD144);
    if (res == -1) return -4;  // dirextory name contains '.'
    if (res == -2) return -5;  // root blocks are full
    if (res == -3) return -6;  // disk data block are full
    free(cpyPtr);
    return 0;
}

int rmdircmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return -1;  // missing operand
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return -2;  // entry doesn't exist
    if (entClus == clus) return -3;  // can't remove present directory
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr != 0x10) return -4;
    if (entnameEq(".", entry.DIR_Name)) return -5;
    size_t entCnt = 0;
    for (unsigned short clus = entClus; clus != 0xfff;
         clus = getNextClus(clus)) {
        unsigned char block[BLOCKSIZE];
        Read_ramFDD_Block(ramFDD144, 31 + clus, block);
        Entry entries[16];
        entCnt += parseEntBlock(block, entries);
    }
    if (entCnt > 2) return -6;
    if (entCnt < 2) return -7;
    int res = rment(entClus, dirClus, ramFDD144);
    return 0;
}

void pwdcmd(unsigned short clus, const unsigned char *ramFDD144) {
    printPath(clus, ramFDD144), printf("\n");
}

void clearcmd() { fputs("\x1b[2J\x1b[H", stdout); }
