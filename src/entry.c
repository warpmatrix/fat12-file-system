#include "entry.h"

size_t parseEnts(const unsigned char *block, Entry *entries) {
    const int BYTSPERENT = 32;
    size_t entCnt = 0;
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

void listEnts(unsigned short fstClus, const unsigned char *ramFDD144) {
    if (fstClus == 0) {
        Entry entries[224];
        size_t entCnt = 0;
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            entCnt += parseEnts(block, entries + entCnt);
            printEnts(entries, entCnt);
        }
    } else {
        for (unsigned short clus = fstClus; clus != 0xfff;
             clus = getNextClus(ramFDD144, clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, clus + 31, block);
            Entry entries[16];
            int entcnt = parseEnts(block, entries);
            printEnts(entries, entcnt);
        }
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

bool entnameEq(const char *str, const char *entname) {
    if (!strcmp(str, ".")) return diskStrEq(".          ", entname, 11);
    if (!strcmp(str, "..")) return diskStrEq("..         ", entname, 11);

    char *strCopy = strdup(str), *cpyPtr = strCopy;
    const char delim[] = ".";

    char *mainFilename = strsep(&strCopy, delim);
    if (!diskStrEq(mainFilename, entname, 8)) {
        free(cpyPtr);
        return false;
    }
    char *extFilename = strsep(&strCopy, delim);
    if (extFilename && !diskStrEq(extFilename, entname + 8, 3)) {
        free(cpyPtr);
        return false;
    }

    free(cpyPtr);
    return true;
}

Entry getEntByName(const char *entname, unsigned short dirClus,
                   const unsigned char *ramFDD144) {
    if (dirClus == 0) {
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            Entry entries[16];
            size_t entCnt = parseEnts(block, entries);
            for (size_t i = 0; i < entCnt; i++)
                if (entnameEq(entname, entries[i].DIR_Name)) return entries[i];
        }
        Entry entry = {.DIR_FstClus = -1};
        return entry;
    } else {
        for (unsigned short clus = dirClus; clus != 0xfff;
             clus = getNextClus(ramFDD144, clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, clus + 31, block);
            Entry entries[16];
            size_t entCnt = parseEnts(block, entries);
            for (size_t i = 0; i < entCnt; i++)
                if (entnameEq(entname, entries[i].DIR_Name)) return entries[i];
        }
        Entry entry = {.DIR_FstClus = -1};
        return entry;
    }
}

Entry getEntByClus(unsigned short entClus, unsigned short dirClus,
                   const unsigned char *ramFDD144) {
    if (dirClus == 0) {
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            Entry entries[16];
            size_t entCnt = parseEnts(block, entries);
            for (size_t i = 0; i < entCnt; i++)
                if (entries[i].DIR_FstClus == entClus) return entries[i];
        }
        Entry entry = {.DIR_FstClus = -1};
        return entry;
    } else {
        for (unsigned short clus = dirClus; clus != 0xfff;
             clus = getNextClus(ramFDD144, clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, clus + 31, block);
            Entry entries[16];
            size_t entCnt = parseEnts(block, entries);
            for (size_t i = 0; i < entCnt; i++)
                if (entries[i].DIR_FstClus == entClus) return entries[i];
        }
        Entry entry = {.DIR_FstClus = -1};
        return entry;
    }
}

// void findEnt(Entry *entry, unsigned short clus, size_t entIdx,
//              const unsigned char *ramFDD144) {
//     size_t base, offset;
//     if (clus == 0) {
//         base = (19 + entIdx / 16) * 512;
//         offset = (entIdx % 16) * 32;
//     } else {
//         base = (31 + clus) * 512;
//         offset = entIdx * 32;
//     }
//     parseEnt(&ramFDD144[base + offset], entry);
// }
