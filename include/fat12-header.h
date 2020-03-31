#ifndef FAT12HEADER_H
#define FAT12HEADER_H

#include "utils.h"

typedef struct Fat12Header {
    char BS_OEMName[8];
    unsigned short BPB_BytsPerSec;
    unsigned char BPB_SecPerClus;
    unsigned short BPB_RsvdSecCnt;
    unsigned char BPB_NumFATs;
    unsigned short BPB_RootEntCnt;
    unsigned short BPB_ToSec16;
    unsigned char BPB_Media;
    unsigned short BPB_FATSz16;
    unsigned short BPB_SecPerTrk;
    unsigned short BPB_NumHeads;
    unsigned int BPB_HiddSec;
    unsigned int BPB_TotSec32;
    unsigned char BS_DrvNum;
    unsigned char BS_Reserved1;
    unsigned char BS_BootSig;
    unsigned int BS_VolID;
    char BS_VolLab[11];
    char BS_FileSysType[8];
} Fat12Header;

Fat12Header parseMbr(const unsigned char *block);
void printMbrInfo(Fat12Header mbr);

#endif
