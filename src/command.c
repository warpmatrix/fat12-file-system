#include "command.h"

int lscmd(unsigned short clus, const char *path,
          const unsigned char *ramFDD144) {
    unsigned short entClus = clus, dirClus = clus;
    if (path) entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return -1;
    if (entClus) {
        Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
        if (entry.DIR_Attr != DIR_ATTR) return -1;
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
        if (entry.DIR_Attr != DIR_ATTR) return -1;
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
    char entname[12];
    getPathEntname(path, entname);
    int res = mkdirent(entname, dirClus, ramFDD144);
    if (res == -1) return -4;  // directory name contains '.'
    if (res == -2) return -5;  // root blocks are full
    if (res == -3) return -6;  // disk data block are full
    return 0;
}

int rmdircmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return -1;  // missing operand
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return -2;  // entry doesn't exist
    if (entClus == clus) return -3;  // can't remove present directory
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr != DIR_ATTR) return -4;
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

int touchcmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return -1;  // missing operand
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (dirClus == (unsigned short)-1) return -2;  // dir doesn't exist
    if (entClus != (unsigned short)-1) return -3;  // file or directory exist
    char entname[12];
    getPathEntname(path, entname);
    int res = mkent(entname, dirClus, ramFDD144);
    if (res == -1) return -4;  // root blocks are full
    if (res == -2) return -5;  // disk data block are full
    return 0;
}

int rmcmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return -1;  // missing operand
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return -2;  // no such file
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr == DIR_ATTR) return -3;   // is a directory
    if (entry.DIR_Attr == PROT_ATTR) return -4;  // operation not permitted
    int res = rment(entClus, dirClus, ramFDD144);
    return 0;
}

int cpcmd(unsigned short clus, const char *path, const char *destPath,
          unsigned char *ramFDD144) {
    if (!path) return -1;      // missing file operand
    if (!destPath) return -2;  // missing destination file
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return -3;  // no such file
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr == DIR_ATTR) return -6; // is a directory
    unsigned short destDirClus = clus;
    unsigned short destEntClus = parsePath(&destDirClus, destPath, ramFDD144);
    // no such destination directory
    if (destDirClus == (unsigned short)-1) return -4;
    char entname[12];
    if (destEntClus != (unsigned short)-1) {
        Entry destEnt = getEntByClus(destEntClus, destDirClus, ramFDD144);
        if (destEnt.DIR_Attr == DIR_ATTR) {
            destDirClus = destEnt.DIR_FstClus;
            destEntClus = -1;
            getPathEntname(path, entname);
        } else
            return -5;  // file or directory exist
    } else
        getPathEntname(destPath, entname);
    int res = cpent(&entry, entname, destDirClus, ramFDD144);
    if (res == -1) return -7;
    if (res == -2) return -8;
    return 0;
}

int catcmd(unsigned short clus, const char *path,
           const unsigned char *ramFDD144) {
    if (!path) return -1;  // missing operand
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return -2;  // no such file
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr == DIR_ATTR) return -3;  // is a directory
    int res = dispFile(&entry, ramFDD144);
    if (res == -1) return -4;  // file error
    return 0;
}

void pwdcmd(unsigned short clus, const unsigned char *ramFDD144) {
    printPath(clus, ramFDD144), printf("\n");
}

void clearcmd() { fputs("\x1b[2J\x1b[H", stdout); }
