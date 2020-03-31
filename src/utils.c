#include "utils.h"

const int BLOCKNUM = 2880;
const int BLOCKSIZE = 512;
const int SIZE = 1474560;  // 2880 * 512

unsigned int parseNum(const unsigned char *str, size_t base, size_t len) {
    unsigned int num = 0;
    for (size_t offset = 0; offset < len; offset++)
        num |= str[base + offset] << (offset * 8);
    return num;
}

unsigned short getNextClus(const unsigned char *ramFDD144,
                           unsigned short clus) {
    const unsigned char *fat = ramFDD144 + BLOCKSIZE;
    unsigned short offset = clus / 2 * 3 - 1;
    if (clus % 2 == 0)
        return ((fat[offset + 1] & 0x0f) << 8) | fat[offset];
    else
        return (fat[offset + 2] << 4) | ((fat[offset + 1] >> 4) & 0x0f);
    // unsigned short fat1Clus = getFatClus(ramFDD144 + BLOCKSIZE, clus);
    // unsigned short fat2Clus = getFatClus(ramFDD144 + BLOCKSIZE * 10, clus);
    // if (fat1Clus == 0xff0) return fat2Clus;
    // return fat1Clus;
}

// unsigned short getEntClus(unsigned short fstClus, const char *entname,
//                           const unsigned char *ramFDD144) {
//     size_t entIdx = findEntIdx(&fstClus, entname, ramFDD144);
//     Entry entry;
//     findEnt(&entry, fstClus, entIdx, ramFDD144);
//     return entry.DIR_FstClus;
// }

unsigned short parsePath(unsigned short *dirClus, const char *path,
                         const unsigned char *ramFDD144) {
    const char delim[] = "/";
    char *pathCopy = strdup(path), *cpyPtr = pathCopy;
    unsigned short newDirClus = *dirClus;
    Entry entry;
    for (const char *entname = strsep(&pathCopy, delim); entname != NULL;
         entname = strsep(&pathCopy, delim)) {
        entry = getEntByName(entname, newDirClus, ramFDD144);
        if (entry.DIR_FstClus == (unsigned short) -1) {
            free(cpyPtr);
            return -1;
        }
        if (pathCopy) newDirClus = entry.DIR_FstClus;
    }
    free(cpyPtr);
    *dirClus = newDirClus;
    return entry.DIR_FstClus;
}

// unsigned short getFatClus(const unsigned char *fat, unsigned short clus) {
//     unsigned short offset = clus / 2 * 3 - 1;
//     if (clus % 2 == 0)
//         return ((fat[offset + 1] & 0x0f) << 8) | fat[offset];
//     else
//         return (fat[offset + 2] << 4) | ((fat[offset + 1] >> 4) & 0x0f);
// }

bool diskStrEq(const char *str, const char *diskStr, int size) {
    for (size_t offset = 0; offset < size; offset++) {
        if (offset < strlen(str) && diskStr[offset] != toupper(str[offset]))
            return false;
        if (offset >= strlen(str) && diskStr[offset] != ' ') return false;
    }
    return true;
}

void parseStr(const unsigned char *block, size_t base, size_t len, char *str) {
    for (size_t offset = 0; offset < len; offset++)
        str[offset] = block[base + offset];
}

void printBlock(const unsigned char *block) {
    const int ENTPERLINE = 32;
    for (size_t i = 0; i < BLOCKSIZE; i++) {
        if (i % ENTPERLINE == 0)
            printf("%02X", block[i]);
        else
            printf(" %02X", block[i]);
        if (i % 8 == 7) printf(" ");
        if (i % ENTPERLINE == ENTPERLINE - 1) printf("\n");
    }
}

void printStr(const char *str, int len) {
    for (size_t offset = 0; offset < len; offset++) printf("%c", str[offset]);
}
