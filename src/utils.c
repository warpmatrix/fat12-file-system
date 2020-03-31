#include "utils.h"

const int BLOCKNUM = 2880;
const int BLOCKSIZE = 512;
const int SIZE = 1474560;  // 2880 * 512

unsigned short getNextClus(const unsigned char *ramFDD144,
                           unsigned short clus) {
    unsigned short fat1Clus = getFatClus(ramFDD144 + BLOCKSIZE, clus);
    return fat1Clus;
}

unsigned short getFatClus(const unsigned char *fat, unsigned short clus) {
    unsigned short offset = clus / 2 * 3 - 1;
    if (clus % 2 == 0)
        return ((fat[offset + 1] & 0x0f) << 8) | fat[offset];
    else
        return (fat[offset + 2] << 4) | ((fat[offset + 1] >> 4) & 0x0f);
}

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

size_t findPath(char **path, unsigned short entClus, const unsigned char *ramFDD144) {
    size_t cnt = 0;
    while (entClus) {
        Entry dirEnt = getEntByName("..", entClus, ramFDD144);
        Entry entry = getEntByClus(entClus, dirEnt.DIR_FstClus, ramFDD144);
        path[cnt] = malloc(12 * sizeof(*path[cnt]));
        for (size_t i = 0; i < 11; i++) {
            if (entry.DIR_Name[i] == ' ') {
                path[cnt][i] = '\0';
                break;
            }
            path[cnt][i] = entry.DIR_Name[i];
            if (i == 10) path[cnt][11] = '\0';
        }
        cnt++;
        entClus = dirEnt.DIR_FstClus;
    }
    return cnt;
}

void printPath(unsigned short clus, const unsigned char *ramFDD144) {
    char **path = malloc((BLOCKNUM - 33) * sizeof(*path));
    size_t cnt = findPath(path, clus, ramFDD144);
    printf("/");
    for (int i = cnt - 1; i >= 0; i--) {
        if (i == 0)
            printf("%s", path[i]);
        else
            printf("%s/", path[i]);
        free(path[i]);
    }
    free(path);
}

bool diskStrEq(const char *str, const char *diskStr, int size) {
    for (size_t offset = 0; offset < size; offset++) {
        if (offset < strlen(str) && diskStr[offset] != toupper(str[offset]))
            return false;
        if (offset >= strlen(str) && diskStr[offset] != ' ') return false;
    }
    return true;
}

unsigned int parseNum(const unsigned char *str, size_t base, size_t len) {
    unsigned int num = 0;
    for (size_t offset = 0; offset < len; offset++)
        num |= str[base + offset] << (offset * 8);
    return num;
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
