#include "test.h"

void printBlock(const unsigned char *block) {
    const int ENTPERLINE = 32;
    for (size_t i = 0; i < BLOCKSIZE; i++) {
        if (i % ENTPERLINE == 0)
            printf("%02X", block[i]);
        else
            printf(" %02X", block[i]);
        if (i % 8 == 7) printf(" ");
        if (i % ENTPERLINE == ENTPERLINE - 1) printf("\n");
    }
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
