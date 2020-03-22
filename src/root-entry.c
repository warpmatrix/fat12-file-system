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
        if (rootEntries[i].DIR_Attr == 0x27) continue;
        printf("DIR_Name: "), printStr(rootEntries[i].DIR_Name, 11);
        printf(",    DIR_Attr: 0x%02X", rootEntries[i].DIR_Attr);
        char time[17];
        parseWriTime(rootEntries[i].DIR_WrtTime, rootEntries[i].DIR_WrtDate,
                     time);
        printf(",    WrtTime: %s", time);
        if (rootEntries[i].DIR_Attr != 0x10)
            printf(",    FileSize: %d Bytes\n", rootEntries[i].DIR_FileSize);
        else
            printf("\n");
    }
}

void parseWriTime(unsigned short DIR_WrtTime, unsigned short DIR_WrtDate,
                  char *time) {
    // DIR_WrtDate: year_offset(7) month(4) day(5)
    int year = 1980 + (DIR_WrtDate >> 16 - 7);  // 1980 + DIR_WrtDate / 2^(16-7)
    int month = (DIR_WrtDate >> 5) & ((1 << 4) - 1);  // DIR_WrtDate / 2^5 % 2^4
    int day = DIR_WrtDate & ((1 << 5) - 1);           // DIR_WrtDate % 2^5
    // DIR_WrtTime: hour(5) min(6) sec/2(5)
    int hour = DIR_WrtTime >> 16 - 5;               // DIR_WrtTime / 2^(16-5)
    int min = (DIR_WrtTime >> 5) & ((1 << 6) - 1);  // DIR_WrtTime / 2^5 % 2^6
    int sec = 2 * (DIR_WrtTime & ((1 << 5) - 1));   // 2 * (DIR_WrtTime % 2^5)
    sprintf(time, "%4d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min,
            sec);
}

bool isLeap(int year) {
    if (year % 400 == 0) return true;
    if (year % 4 == 0 && year % 100 != 0) return true;
    return false;
}

int daysPerMon(int year, int month) {
    switch (month) {
        case 2:
            return isLeap(year) ? 29 : 28;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        default:
            return 31;
    }
}

void printStr(const char *str, int len) {
    for (size_t offset = 0; offset < len; offset++) printf("%c", str[offset]);
}
