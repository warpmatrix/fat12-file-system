#ifndef ROOTENTRY_H
#define ROOTENTRY_H

#include <stdio.h>
#include <stdbool.h>
#include "structrue.h"

typedef struct RootEntry {
    char DIR_Name[11];
    unsigned char DIR_Attr;
    char Reserve[10];
    unsigned short DIR_WrtTime;
    unsigned short DIR_WrtDate;
    unsigned short DIR_FstClus;
    unsigned int DIR_FileSize;
} RootEntry;

int parseEnts(const unsigned char *block, RootEntry *rootEntries);
void parseEnt(const unsigned char *block, RootEntry *rootEntry);
void printEnts(const RootEntry *rootEntries, int entCnt);
void parseWriTime(unsigned short DIR_WrtTime, unsigned short DIR_WrtDate, char *time);
bool isLeap(int year);
int daysPerMon(int year, int month);
void printStr(const char *str, int len);

#endif
