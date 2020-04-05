#ifndef ENTRY_H
#define ENTRY_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fat.h"
#include "io.h"
#include "utils.h"

#define BYTSPERENT 32

#define DIR_ATTR 0x10
#define PROT_ATTR 0x27

typedef struct Entry {
    char DIR_Name[11];
    unsigned char DIR_Attr;
    char Reserve[10];
    unsigned short DIR_WrtTime;
    unsigned short DIR_WrtDate;
    unsigned short DIR_FstClus;
    unsigned int DIR_FileSize;
} Entry;

Entry parseEntStr(const unsigned char *entryStr);
void parseEnt(const Entry *entry, unsigned char *entStr);

Entry mknewEnt(const char *entname, unsigned char attr, time_t secs,
               unsigned short fstClus, unsigned int size);
bool markEntDel(unsigned short entClus, unsigned char *block);
int rment(unsigned short entClus, unsigned short dirClus,
          unsigned char *ramFDD144);

void parseTime(time_t time, unsigned short *wrtTime, unsigned short *wrtDate);
void parseWriTime(unsigned short DIR_WrtTime, unsigned short DIR_WrtDate,
                  char *time);

bool isLeap(int year);
int daysPerMon(int year, int month);

bool entnameEq(const char *str, const char *entname);

#endif
