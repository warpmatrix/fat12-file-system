#ifndef ENTRY_H
#define ENTRY_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

extern const size_t BYTSPERENT;

#include "fat.h"
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

size_t parseEntBlock(const unsigned char *block, Entry *entries);
Entry parseEntStr(const unsigned char *entryStr);

void parseEnt(const Entry *entry, unsigned char *entStr);
void parseEnts(const Entry *entries, size_t entCnt, unsigned char *block);

size_t getFreeEntIdx(unsigned char *block);
size_t getDirFreeEnt(size_t *blockIdx, unsigned short dirClus,
                     unsigned char *ramFDD144);

int mknewDir(const char *entname, unsigned short dirClus,
             unsigned char *ramFDD144);
Entry mknewEnt(const char *entname, unsigned char attr, time_t secs,
               unsigned short fstClus, unsigned int size);
void parseTime(time_t time, unsigned short *wrtTime, unsigned short *wrtDate);

void printEnts(const Entry *entries, int entCnt);
void listEnts(unsigned short fstClus, const unsigned char *ramFDD144);
void parseWriTime(unsigned short DIR_WrtTime, unsigned short DIR_WrtDate,
                  char *time);

bool isLeap(int year);
int daysPerMon(int year, int month);

bool entnameEq(const char *str, const char *entname);

Entry getEntByName(const char *entname, unsigned short dirClus,
                   const unsigned char *ramFDD144);
Entry getEntByClus(unsigned short entClus, unsigned short dirClus,
                   const unsigned char *ramFDD144);

#endif
