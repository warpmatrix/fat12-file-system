#include "entry.h"

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

void parseEnt(const Entry *entry, unsigned char *entStr) {
    parseEntCharStr(entry->DIR_Name, entStr, 0, 11);
    parseEntNum(entry->DIR_Attr, entStr, 11, 1);
    parseEntCharStr(entry->Reserve, entStr, 12, 10);
    parseEntNum(entry->DIR_WrtTime, entStr, 22, 2);
    parseEntNum(entry->DIR_WrtDate, entStr, 24, 2);
    parseEntNum(entry->DIR_FstClus, entStr, 26, 2);
    parseEntNum(entry->DIR_FileSize, entStr, 28, 4);
}

Entry getEnt(const char *entname, unsigned char attr, time_t wrtTime,
             unsigned short fstClus, unsigned int fileSize) {
    Entry entry;
    if (!strcmp(entname, "."))
        strcpy(entry.DIR_Name, ".          ");
    else if (!strcmp(entname, ".."))
        strcpy(entry.DIR_Name, "..         ");
    else {
        char *strCopy = strdup(entname), *cpyPtr = strCopy;
        const char delim[] = ".";
        char *mainFilename = strsep(&strCopy, delim);
        for (size_t i = 0; i < 8; i++)
            if (i < strlen(mainFilename))
                entry.DIR_Name[i] = mainFilename[i];
            else
                entry.DIR_Name[i] = ' ';
        char *extFilename = strsep(&strCopy, delim);
        for (size_t i = 0; i < 3; i++)
            if (extFilename && i < strlen(extFilename))
                entry.DIR_Name[8 + i] = extFilename[i];
            else
                entry.DIR_Name[8 + i] = ' ';
        entry.DIR_Name[11] = '\0';
        free(cpyPtr);
    }
    entry.DIR_Attr = attr;
    memset(entry.Reserve, 0, 10);
    parseTime(wrtTime, &entry.DIR_WrtTime, &entry.DIR_WrtDate);
    entry.DIR_FstClus = fstClus;
    entry.DIR_FileSize = fileSize;
    return entry;
}

void printEntInfo(const Entry *entry) {
    if (entry->DIR_Attr == DIR_ATTR) printf("\x1b[34;1m");
    char filename[12];
    parseEntName(entry, filename);
    printf("%-12s", filename);
    printf("\x1b[0m");
    printf("  0x%02X", entry->DIR_Attr);
    char time[20];
    parseWriTime(entry->DIR_WrtTime, entry->DIR_WrtDate, time);
    printf("  %s", time);
    if (entry->DIR_Attr != DIR_ATTR) printf("  %-7d Bytes", entry->DIR_FileSize);
    printf("\n");
}

void parseEntName(const Entry *entry, char *filename) {
    if (!strcmp(".          ", entry->DIR_Name)) {
        strcpy(filename, ".");
        return;
    }
    if (!strcmp("..         ", entry->DIR_Name)) {
        strcpy(filename, "..");
        return;
    }
    size_t len = 0;
    for (size_t i = 0; i < 8; i++) {
        if (entry->DIR_Name[i] == ' ') break;
        filename[i] = entry->DIR_Name[len++];
    }
    if (entry->DIR_Name[8] == ' ') {
        filename[len] = '\0';
    } else
        filename[len++] = '.';
    for (size_t i = 8; i < 11; i++) {
        if (entry->DIR_Name[i] == ' ') break;
        filename[len++] = entry->DIR_Name[i];
    }
    filename[len] = '\0';
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
