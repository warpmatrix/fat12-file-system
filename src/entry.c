#include "entry.h"

int parseEnts(const unsigned char *block, Entry *entries) {
    const int BYTSPERENT = 32;
    int entCnt = 0;
    for (size_t entIdx = 0; entIdx < BLOCKSIZE / BYTSPERENT; entIdx++) {
        if (block[entIdx * BYTSPERENT] == 0) break;
        if (block[entIdx * BYTSPERENT] == 0xe5) continue;
        parseEnt(block + entIdx * BYTSPERENT, entries + entCnt++);
    }
    return entCnt;
}

void parseEnt(const unsigned char *entryStr, Entry *entry) {
    parseStr(entryStr, 0, 11, entry->DIR_Name);
    entry->DIR_Attr = parseNum(entryStr, 11, 1);
    parseStr(entryStr, 12, 10, entry->Reserve);
    entry->DIR_WrtTime = parseNum(entryStr, 22, 2);
    entry->DIR_WrtDate = parseNum(entryStr, 24, 2);
    entry->DIR_FstClus = parseNum(entryStr, 26, 2);
    entry->DIR_FileSize = parseNum(entryStr, 28, 4);
}

void printEnts(const Entry *entries, int entCnt) {
    for (size_t i = 0; i < entCnt; i++) {
        if (entries[i].DIR_Attr == 0x27) continue;
        printf("DIR_Name: "), printStr(entries[i].DIR_Name, 11);
        printf(",    DIR_Attr: 0x%02X", entries[i].DIR_Attr);
        char time[20];
        parseWriTime(entries[i].DIR_WrtTime, entries[i].DIR_WrtDate, time);
        printf(",    WrtTime: %s", time);
        if (entries[i].DIR_Attr != 0x10)
            printf(",    FileSize: %d Bytes\n", entries[i].DIR_FileSize);
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

int findDirClus(const Entry *entries, int entCnt, const char *dirname) {
    for (size_t i = 0; i < entCnt; i++)
        if (dirnameEq(dirname, entries[i].DIR_Name) &&
            entries[i].DIR_Attr == 0x10) {
            return entries[i].DIR_FstClus;
        }
    return -1;
}

bool dirnameEq(const char *dirname, const char *entDirname) {
    size_t len = strlen(dirname);
    for (size_t i = 0; i < len; i++)
        if (toupper(dirname[i]) != entDirname[i]) return false;
    if (entDirname[len] == ' ')
        return true;
    else
        return false;
}
