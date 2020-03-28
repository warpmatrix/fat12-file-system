#include "command.h"

int listEnts(unsigned short fstClus, const unsigned char *ramFDD144) {
    if (fstClus == 0) {
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            Entry entries[16];
            int entCnt = parseEnts(block, entries);
            printEnts(entries, entCnt);
        }
    } else {
        for (unsigned short clus = fstClus; clus != 0xfff;
             clus = getNextClus(ramFDD144, clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, clus + 31, block);
            Entry entries[16];
            int entcnt = parseEnts(block, entries);
            printEnts(entries, entcnt);
        }
    }
    return 0;
}

int changeDir(unsigned short *origClus, const unsigned char *ramFDD144) {
    // char entname[9];
    // scanf("%s", entname);
    // if (*origClus == 0) {
    //     for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
    //         unsigned char block[BLOCKSIZE];
    //         Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
    //         Entry entries[16];
    //         int entCnt = parseEnts(block, entries);
    //         int newClus = findDirClus(entries, entCnt, entname);
    //         if (newClus != -1) {
    //             *origClus = newClus;
    //             return 0;
    //         }
    //         if (entCnt < 16) break;
    //     }
    //     return -1;
    // } else {
    //     for (unsigned short clus = *origClus; clus != 0xfff;
    //          clus = getNextClus(ramFDD144, clus)) {
    //         unsigned char block[BLOCKSIZE];
    //         Read_ramFDD_Block(ramFDD144, clus + 31, block);
    //         Entry entries[16];
    //         int entCnt = parseEnts(block, entries);
    //         int newClus = findDirClus(entries, entCnt, entname);
    //         if (newClus != -1) {
    //             *origClus = newClus;
    //             return 0;
    //         }
    //     }
    //     return -1;
    // }
}
