#include "fat12-header.h"

void parseMbr(const unsigned char *block, Fat12Header *mbr) {
    parseStr(block, 3, 8, mbr->BS_OEMName);
    mbr->BPB_BytsPerSec = parseNum(block, 11, 2);
    mbr->BPB_SecPerClus = parseNum(block, 13, 1);
    mbr->BPB_RsvdSecCnt = parseNum(block, 14, 2);
    mbr->BPB_NumFATs = parseNum(block, 16, 1);
    mbr->BPB_RootEntCnt = parseNum(block, 17, 2);
    mbr->BPB_ToSec16 = parseNum(block, 19, 2);
    mbr->BPB_Media = parseNum(block, 21, 1);
    mbr->BPB_FATSz16 = parseNum(block, 22, 2);
    mbr->BPB_SecPerTrk = parseNum(block, 24, 2);
    mbr->BPB_NumHeads = parseNum(block, 26, 2);
    mbr->BPB_HiddSec = parseNum(block, 28, 4);
    mbr->BPB_TotSec32 = parseNum(block, 32, 4);
    mbr->BS_DrvNum = parseNum(block, 36, 1);
    mbr->BS_Reserved1 = parseNum(block, 37, 1);
    mbr->BS_BootSig = parseNum(block, 38, 1);
    mbr->BS_VolID = parseNum(block, 39, 4);
    parseStr(block, 43, 11, mbr->BS_VolLab);
    parseStr(block, 54, 8, mbr->BS_FileSysType);
}
