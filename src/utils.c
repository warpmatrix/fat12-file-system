#include "utils.h"

const int BLOCKSIZE = 512;
const int SIZE = 1474560;  // 2880 * 512

bool diskStrEq(const char *str, const char *diskStr, int size) {
    for (size_t offset = 0; offset < size; offset++) {
        if (offset < strlen(str) && diskStr[offset] != str[offset])
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

void parseEntNum(unsigned int entNum, unsigned char *entStr, size_t base, size_t len) {
    for (size_t offset = 0; offset < len; offset++)
        entStr[base + offset] = entNum & 0xff, entNum >>= 8;
}

void parseEntCharStr(const char *entCharStr, unsigned char *entStr, size_t base, size_t len) {
    for (size_t offset = 0; offset < len; offset++)
        entStr[base + offset] = entCharStr[offset];
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

unsigned short parsePath(unsigned short *dirClus, const char *path,
                         const unsigned char *ramFDD144) {
    const char delim[] = "/";
    char *pathCopy = strdup(path), *cpyPtr = pathCopy;
    Entry entry;
    for (const char *entname = strsep(&pathCopy, delim); entname != NULL;
         entname = strsep(&pathCopy, delim)) {
        entry = getEntByName(entname, *dirClus, ramFDD144);
        if (pathCopy) *dirClus = entry.DIR_FstClus;
        if (entry.DIR_FstClus == (unsigned short)-1) break;
    }
    free(cpyPtr);
    return entry.DIR_FstClus;
}

size_t findPath(char **path, unsigned short entClus,
                const unsigned char *ramFDD144) {
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
