#include "entry.h"

size_t parseEntBlock(const unsigned char *block, Entry *entries) {
    const int BYTSPERENT = 32;
    size_t entCnt = 0;
    for (size_t entIdx = 0; entIdx < BLOCKSIZE / BYTSPERENT; entIdx++) {
        if (block[entIdx * BYTSPERENT] == 0) break;
        if (block[entIdx * BYTSPERENT] == 0xe5) continue;
        entries[entCnt++] = parseEntStr(block + entIdx * BYTSPERENT);
    }
    return entCnt;
}

Entry parseEntStr(const unsigned char *entryStr) {
    Entry entry;
    parseStr(entryStr, 0, 11, entry.DIR_Name);
    entry.DIR_Attr = parseNum(entryStr, 11, 1);
    parseStr(entryStr, 12, 10, entry.Reserve);
    entry.DIR_WrtTime = parseNum(entryStr, 22, 2);
    entry.DIR_WrtDate = parseNum(entryStr, 24, 2);
    entry.DIR_FstClus = parseNum(entryStr, 26, 2);
    entry.DIR_FileSize = parseNum(entryStr, 28, 4);
    return entry;
}

int mknewDir(const char *entname, unsigned short dirClus,
             unsigned char *ramFDD144) {
    unsigned short newEntClus; // = getFreeClus();
    Entry entries[2];
    time_t wrtTime = time(NULL);
    entries[0] = mknewEnt(".", 0x10, wrtTime, newEntClus, 0);
    entries[1] = mknewEnt("..", 0x10, wrtTime, dirClus, 0);
    unsigned char block[BLOCKSIZE];
    // parseEnts(entries, block);
}

Entry mknewEnt(const char *entname, unsigned char attr, time_t wrtTime,
               unsigned short fstClus, unsigned int fileSize) {
    Entry entry;
    size_t len = strlen(entname);
    for (size_t i = 0; i < 11; i++)
        if (i < len)
            entry.DIR_Name[i] = entname[i];
        else
            entry.DIR_Name[i] = ' ';
    entry.DIR_Attr = attr;
    parseTime(wrtTime, &entry.DIR_WrtTime, &entry.DIR_WrtDate);
    entry.DIR_FstClus = fstClus;
    entry.DIR_FileSize = fileSize;
    return entry;
}

void parseTime(time_t timer, unsigned short *wrtTime, unsigned short *wrtDate) {
    struct tm *time = localtime(&timer);
    unsigned short hour = time->tm_hour;
    unsigned short min = time->tm_min;
    unsigned short sec = time->tm_sec;
    *wrtTime = (hour & 0x1f) << 11 | (min & 0x3f) << 5 | ((sec >> 1) & 0x1f);
    
    unsigned short yearOfst = time->tm_year + 1900 - 1980;
    unsigned short month = time->tm_mon + 1;
    unsigned short day = time->tm_mday;
    *wrtDate = (yearOfst & 0x7f) << 9 | (month & 0xf) << 5 | (day & 0x1f);
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
            entCnt += parseEntBlock(block, entries + entCnt);
        }
        printEnts(entries, entCnt), printf("\n");
    } else {
        for (unsigned short clus = fstClus; clus != 0xfff;
             clus = getNextClus(ramFDD144, clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, clus + 31, block);
            Entry entries[16];
            int entcnt = parseEntBlock(block, entries);
            printEnts(entries, entcnt), printf("\n");
        }
    }
}

void parseWriTime(unsigned short DIR_WrtTime, unsigned short DIR_WrtDate,
                  char *time) {
    // DIR_WrtDate: year_offset(7) month(4) day(5)
    int year = 1980 + (DIR_WrtDate >> 9);   // 1980 + DIR_WrtDate / 2^(4+5)
    int month = (DIR_WrtDate >> 5) & 0x0f;  // DIR_WrtDate / 2^5 % 2^4
    int day = DIR_WrtDate & 0x1f;           // DIR_WrtDate % 2^5
    // DIR_WrtTime: hour(5) min(6) sec/2(5)
    int hour = DIR_WrtTime >> 11;         // DIR_WrtTime / 2^(6+5)
    int min = (DIR_WrtTime >> 5) & 0x3f;  // DIR_WrtTime / 2^5 % 2^6
    int sec = (DIR_WrtTime & 0x1f) << 1;  // 2 * (DIR_WrtTime % 2^5)
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
            size_t entCnt = parseEntBlock(block, entries);
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
            size_t entCnt = parseEntBlock(block, entries);
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
            size_t entCnt = parseEntBlock(block, entries);
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
            size_t entCnt = parseEntBlock(block, entries);
            for (size_t i = 0; i < entCnt; i++)
                if (entries[i].DIR_FstClus == entClus) return entries[i];
        }
        Entry entry = {.DIR_FstClus = -1};
        return entry;
    }
}
