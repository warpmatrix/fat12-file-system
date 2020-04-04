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
    if (res == -1) return -4;  // root blocks are full
    if (res == -2) return -5;  // disk data block are full
    free(cpyPtr);
    return 0;
}

int rmdircmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return -1;  // missing operand
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return -2;  // entry doesn't exist
    // int res = rmdir(entClus, dirClus, ramFDD144);
    // if (res == -1) return -4;
    // if (res == -2) return -5;
    return 0;
}

void pwdcmd(unsigned short clus, const unsigned char *ramFDD144) {
    printPath(clus, ramFDD144), printf("\n");
}

void clearcmd() { fputs("\x1b[2J\x1b[H", stdout); }
