#include "command.h"

int listEnts(unsigned short fstClus, const char *path,
             const unsigned char *ramFDD144) {
    if (path != NULL) fstClus = parsePath(path, fstClus, ramFDD144);
    if (fstClus == 0) {
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            Entry entries[16];
            int entCnt = parseEnts(block, entries);
            printEnts(entries, entCnt);
        }
        return 0;
    } else {
        for (unsigned short clus = fstClus; clus != 0xfff;
             clus = getNextClus(ramFDD144, clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, clus + 31, block);
            Entry entries[16];
            int entcnt = parseEnts(block, entries);
            printEnts(entries, entcnt);
        }
        return 0;
    }
    return -1;
}

int changeDir(unsigned short *origClus, const char *path,
              const unsigned char *ramFDD144) {}
