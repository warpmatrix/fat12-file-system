#ifndef ENTRY_H
#define ENTRY_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "utils.h"

typedef struct Entry {
    char DIR_Name[11];
    unsigned char DIR_Attr;
    char Reserve[10];
    unsigned short DIR_WrtTime;
    unsigned short DIR_WrtDate;
    unsigned short DIR_FstClus;
    unsigned int DIR_FileSize;
} Entry;

int parseEnts(const unsigned char *block, Entry *entries);
void parseEnt(const unsigned char *entryStr, Entry *entry);
void printEnts(const Entry *entries, int entCnt);
void parseWriTime(unsigned short DIR_WrtTime, unsigned short DIR_WrtDate, char *time);

bool isLeap(int year);
int daysPerMon(int year, int month);

int findDirClus(const Entry *entries, int entCnt, const char *dirname);
bool dirnameEq(const char *dirname, const char *entDirname);
#endif
