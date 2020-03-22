#include "root-entry.h"

int parseEnts(const unsigned char *block, RootEntry *rootEntries) {
    const int BYTSPERENT = 32;
    int entCnt = 0;
    for (size_t entIdx = 0; entIdx < BLOCKSIZE / BYTSPERENT; entIdx++) {
        if (block[entIdx * BYTSPERENT] == 0 ||
            block[entIdx * BYTSPERENT] == 0xe5)
            continue;
        parseEnt(block + entIdx * BYTSPERENT, rootEntries + entCnt++);
    }
    return entCnt;
}

void parseEnt(const unsigned char *block, RootEntry *rootEntry) {
    parseStr(block, 0, 11, rootEntry->DIR_Name);
    rootEntry->DIR_Attr = parseNum(block, 11, 1);
    parseStr(block, 12, 10, rootEntry->Reserve);
    rootEntry->DIR_WrtTime = parseNum(block, 22, 2);
    rootEntry->DIR_WrtDate = parseNum(block, 24, 2);
    rootEntry->DIR_FstClus = parseNum(block, 26, 2);
    rootEntry->DIR_FileSize = parseNum(block, 28, 4);
}

void printEnts(const RootEntry *rootEntries, int entCnt) {
    for (size_t i = 0; i < entCnt; i++) {
        printf("DIR_Name: "), printStr(rootEntries[i].DIR_Name, 11);
        printf(", DIR_Attr: 0x%02X", rootEntries[i].DIR_Attr);
        char time[16];
        parseWriTime(rootEntries[i].DIR_WrtTime, rootEntries[i].DIR_WrtDate, time);
        printf(", DIR_WrtTime: %s", time);
        // printf(", DIR_WrtTime: 0x%X", rootEntries[i].DIR_WrtTime);
        // printf(", DIR_WrtDate: 0x%X", rootEntries[i].DIR_WrtDate);
        printf(", DIR_FileSize: 0x%X", rootEntries[i].DIR_FileSize);
        printf("\n");
    }
}

void parseWriTime(unsigned short DIR_WrtTime, unsigned short DIR_WrtDate, char *time) {

}

void printStr(const char *str, int len) {
    for (size_t offset = 0; offset < len; offset++)
        printf("%c", str[offset]);
}
