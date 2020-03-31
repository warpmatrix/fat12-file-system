#ifndef ENTRY_H
#define ENTRY_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "io.h"
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

size_t parseEnts(const unsigned char *block, Entry *entries);
void parseEnt(const unsigned char *entryStr, Entry *entry);
void printEnts(const Entry *entries, int entCnt);
void listEnts(unsigned short fstClus, const unsigned char *ramFDD144);
void parseWriTime(unsigned short DIR_WrtTime, unsigned short DIR_WrtDate, char *time);

bool isLeap(int year);
int daysPerMon(int year, int month);

bool entnameEq(const char *str, const char *entname);
Entry getEntByName(const char *entname, unsigned short dirClus,
                   const unsigned char *ramFDD144);
Entry getEntByClus(unsigned short entClus, unsigned short dirClus,
                   const unsigned char *ramFDD144);

#endif
