#include "fat12-header.h"

Fat12Header parseMbr(const unsigned char *block) {
    Fat12Header mbr;
    parseStr(block, 3, 8, mbr.BS_OEMName);
    mbr.BPB_BytsPerSec = parseNum(block, 11, 2);
    mbr.BPB_SecPerClus = parseNum(block, 13, 1);
    mbr.BPB_RsvdSecCnt = parseNum(block, 14, 2);
    mbr.BPB_NumFATs = parseNum(block, 16, 1);
    mbr.BPB_RootEntCnt = parseNum(block, 17, 2);
    mbr.BPB_ToSec16 = parseNum(block, 19, 2);
    mbr.BPB_Media = parseNum(block, 21, 1);
    mbr.BPB_FATSz16 = parseNum(block, 22, 2);
    mbr.BPB_SecPerTrk = parseNum(block, 24, 2);
    mbr.BPB_NumHeads = parseNum(block, 26, 2);
    mbr.BPB_HiddSec = parseNum(block, 28, 4);
    mbr.BPB_TotSec32 = parseNum(block, 32, 4);
    mbr.BS_DrvNum = parseNum(block, 36, 1);
    mbr.BS_Reserved1 = parseNum(block, 37, 1);
    mbr.BS_BootSig = parseNum(block, 38, 1);
    mbr.BS_VolID = parseNum(block, 39, 4);
    parseStr(block, 43, 11, mbr.BS_VolLab);
    parseStr(block, 54, 8, mbr.BS_FileSysType);
    return mbr;
}

void printMbrInfo(Fat12Header mbr) {
    printf("BS_OEMName: "), printStr(mbr.BS_OEMName, 8), printf("\n");
    printf("BPB_BytsPerSec: 0x%X\n", mbr.BPB_BytsPerSec);
    printf("BPB_SecPerClus: 0x%X\n", mbr.BPB_SecPerClus);
    printf("BPB_RsvdSecCnt: 0x%X\n", mbr.BPB_RsvdSecCnt);
    printf("BPB_NumFATs: 0x%X\n", mbr.BPB_NumFATs);
    printf("BPB_RootEntCnt: 0x%X\n", mbr.BPB_RootEntCnt);
    printf("BPB_ToSec16: 0x%X\n", mbr.BPB_ToSec16);
    printf("BPB_Media: 0x%X\n", mbr.BPB_Media);
    printf("BPB_FATSz16: 0x%X\n", mbr.BPB_FATSz16);
    printf("BPB_SecPerTrk: 0x%X\n", mbr.BPB_SecPerTrk);
    printf("BPB_NumHeads: 0x%X\n", mbr.BPB_NumHeads);
    printf("BPB_HiddSec: 0x%X\n", mbr.BPB_HiddSec);
    printf("BPB_TotSec32: 0x%X\n", mbr.BPB_TotSec32);
    printf("BS_DrvNum: 0x%X\n", mbr.BS_DrvNum);
    printf("BS_Reserved1: 0x%X\n", mbr.BS_Reserved1);
    printf("BS_BootSig: 0x%X\n", mbr.BS_BootSig);
    printf("BS_VolID: 0x%X\n", mbr.BS_VolID);
    printf("BS_VolLab: "), printStr(mbr.BS_VolLab, 11), printf("\n");
    printf("BS_FileSysType: "), printStr(mbr.BS_FileSysType, 8), printf("\n");
}
