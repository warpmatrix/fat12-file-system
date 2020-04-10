#include "command.h"

int lscmd(unsigned short clus, const char *path,
          const unsigned char *ramFDD144) {
    unsigned short entClus = clus, dirClus = clus;
    if (path) entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return NO_FILE_DIR;
    if (entClus) {
        Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
        if (entry.DIR_Attr != DIR_ATTR) return NO_FILE_DIR;
    }
    listEnts(entClus, ramFDD144);
    return 0;
}

int cdcmd(unsigned short *clus, const char *path,
          const unsigned char *ramFDD144) {
    unsigned short entClus = 0, dirClus = *clus;
    if (path) entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return NO_FILE_DIR;
    if (entClus) {
        Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
        if (entry.DIR_Attr != DIR_ATTR) return NO_FILE_DIR;
    }
    *clus = entClus;
    return 0;
}

int mkdircmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return MISS_OPERAND;
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (dirClus == (unsigned short)-1) return NO_FILE_DIR;
    if (entClus != (unsigned short)-1) return FILE_DIR_XST;
    char entname[12];
    getPathEntname(path, entname);
    int res = mkdirent(entname, dirClus, ramFDD144);
    if (res == ROOT_IS_FULL) return ROOT_IS_FULL;
    if (res == DISK_IS_FULL) return DISK_IS_FULL;
    return 0;
}

int rmdircmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return MISS_OPERAND;
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return NO_FILE_DIR;
    if (entClus == clus) return NOT_RM_PRES_DIR;
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr != DIR_ATTR) return NO_FILE_DIR;
    if (entnameEq(".", entry.DIR_Name)) return INVALID_ARGUMET;
    size_t entCnt = 0;
    for (unsigned short clus = entClus; clus != 0xfff;
         clus = getNextClus(clus)) {
        unsigned char block[BLOCKSIZE];
        Read_ramFDD_Block(ramFDD144, 31 + clus, block);
        Entry entries[16];
        entCnt += parseEntBlock(block, entries);
    }
    if (entCnt > 2) return DIR_NOT_EMPTY;
    if (entCnt < 2) return FILE_ERROR;
    int res = rment(entClus, dirClus, ramFDD144);
    return 0;
}

int touchcmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return MISS_OPERAND;
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (dirClus == (unsigned short)-1) return NO_FILE_DIR;
    if (entClus != (unsigned short)-1) return FILE_DIR_XST;
    char entname[12];
    getPathEntname(path, entname);
    int res = mkent(entname, dirClus, ramFDD144);
    if (res == -1) return ROOT_IS_FULL;
    if (res == -2) return DISK_IS_FULL;
    return res;
}

int rmcmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return MISS_OPERAND;
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return NO_FILE_DIR;
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr == DIR_ATTR) return IS_DIRECTORY;
    if (entry.DIR_Attr == PROT_ATTR) return OPER_NOT_PERM;
    int res = rment(entClus, dirClus, ramFDD144);
    return 0;
}

int cpcmd(unsigned short clus, const char *path, const char *destPath,
          unsigned char *ramFDD144) {
    if (!path) return MISS_OPERAND;
    if (!destPath) return MISS_DEST_FILE;
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return NO_FILE_DIR;
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr == DIR_ATTR) return IS_DIRECTORY;
    unsigned short destDirClus = clus;
    unsigned short destEntClus = parsePath(&destDirClus, destPath, ramFDD144);
    if (destDirClus == (unsigned short)-1) return NO_FILE_DIR;
    char entname[12];
    if (destEntClus != (unsigned short)-1) {
        Entry destEnt = getEntByClus(destEntClus, destDirClus, ramFDD144);
        if (destEnt.DIR_Attr == DIR_ATTR) {
            destDirClus = destEnt.DIR_FstClus;
            destEntClus = -1;
            getPathEntname(path, entname);
        } else
            return FILE_DIR_XST;
    } else
        getPathEntname(destPath, entname);
    int res = cpent(&entry, entname, destDirClus, ramFDD144);
    if (res == -1) return ROOT_IS_FULL;
    if (res == -2) return DISK_IS_FULL;
    if (res == -3) return FILE_ERROR;
    return 0;
}

int catcmd(unsigned short clus, const char *path,
           const unsigned char *ramFDD144) {
    if (!path) return MISS_OPERAND;
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return NO_FILE_DIR;
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr == DIR_ATTR) return IS_DIRECTORY;
    int res = dispFile(&entry, ramFDD144);
    if (res == -1) return FILE_ERROR;
    return 0;
}

int editcmd(unsigned short clus, const char *path, unsigned char *ramFDD144) {
    if (!path) return MISS_OPERAND;
    unsigned short dirClus = clus;
    unsigned short entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return NO_FILE_DIR;
    Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
    if (entry.DIR_Attr == DIR_ATTR) return IS_DIRECTORY;
    int res = editFile(&entry, dirClus, ramFDD144);
    if (res == -1) return FILE_ERROR;
    if (res == -2) return DISK_IS_FULL;
    return 0;
}

int treecmd(unsigned short clus, const char *path,
            const unsigned char *ramFDD144) {
    unsigned short entClus = clus, dirClus = clus;
    if (path) entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return NO_FILE_DIR;
    if (entClus) {
        Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
        if (entry.DIR_Attr != DIR_ATTR) return NO_FILE_DIR;
    }
    printPath(entClus, ramFDD144), printf("\n");
    Stack lastEnt;
    initStack(&lastEnt);
    printTree(entClus, &lastEnt, ramFDD144);
    freeStack(&lastEnt);
    return 0;
}

int pwdcmd(unsigned short clus, const unsigned char *path,
           const unsigned char *ramFDD144) {
    unsigned short dirClus = clus, entClus = clus;
    if (path) entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short)-1) return NO_FILE_DIR;
    char targetPath[BLOCKNUM - 33][12];
    size_t cnt = findPath(targetPath, entClus, ramFDD144);
    for (int i = cnt - 1; i >= 0; i--)
        if (i == 0 || i == cnt - 1)
            printf("%s", targetPath[i]);
        else
            printf("%s/", targetPath[i]);
    printf("\n");
    return 0;
}

int clearcmd(void) {
    fputs("\x1b[2J\x1b[H", stdout);
    return 0;
}
