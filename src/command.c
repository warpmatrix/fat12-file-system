#include "command.h"

void listEnts(int clus, const unsigned char *ramFDD144) {
    unsigned char block[BLOCKSIZE];
    if (clus == 0) {
        printf("Root Entries:\n");
        Entry rootEntries[224];
        int entCnt = 0;
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            entCnt += parseEnts(block, rootEntries + entCnt);
        }
        printEnts(rootEntries, entCnt);
    }
}