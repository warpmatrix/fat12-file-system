#include "fat.h"

static unsigned char fat[BLOCKSIZE * 9];

int initFat(const unsigned char *ramFDD144) {
    size_t fat1BaseSec = 1, fat2BaseSec = 10;
    for (size_t secOfst = 0; secOfst < 9; secOfst++) {
        unsigned char block1[BLOCKSIZE];
        Read_ramFDD_Block(ramFDD144, fat1BaseSec + secOfst, block1);
        unsigned char block2[BLOCKSIZE];
        Read_ramFDD_Block(ramFDD144, fat2BaseSec + secOfst, block2);
        for (size_t offset = 0; offset < BLOCKSIZE; offset++) {
            if (block1[offset] != block2[offset]) return -1;
            fat[secOfst * BLOCKSIZE + offset] = block1[offset];
        }
    }
    return 0;
}

void writeFats(unsigned char *ramFDD144) {
    size_t fat1BaseSec = 1;
    size_t fat2BaseSec = 10;
    for (size_t secOfst = 0; secOfst < 9; secOfst++) {
        unsigned char block[BLOCKSIZE];
        for (size_t offset = 0; offset < BLOCKSIZE; offset++) {
            block[offset] = fat[secOfst * BLOCKSIZE + offset];
        }
        Write_ramFDD_Block(block, ramFDD144, fat1BaseSec + secOfst);
        Write_ramFDD_Block(block, ramFDD144, fat2BaseSec + secOfst);
    }
}

unsigned short getNextClus(unsigned short clus) {
    unsigned short offset = clus / 2 * 3;
    unsigned short nextClus;
    if (clus % 2 == 0)
        nextClus = ((fat[offset + 1] & 0x0f) << 8) | fat[offset];
    else
        nextClus = (fat[offset + 2] << 4) | ((fat[offset + 1] >> 4) & 0x0f);
    return nextClus;
}

unsigned short getFreeClus() {
    for (unsigned short clus = 2; clus < BLOCKNUM - 31; clus++)
        if (getNextClus(clus) == 0) return clus;
    return -1;
}

void addEntClus(unsigned short fstClus, unsigned short newClus) {
    if (getNextClus(fstClus) == 0) {
        setFatClus(fstClus, 0xfff);
        return;
    }
    unsigned short endClus = fstClus;
    for (unsigned short nextClus = getNextClus(fstClus); nextClus != 0xfff;
         endClus = nextClus, nextClus = getNextClus(nextClus))
        ;
    setFatClus(newClus, 0xfff);
    setFatClus(endClus, newClus);
}

void setFatClus(unsigned short clus, unsigned short newClus) {
    unsigned short offset = clus / 2 * 3;
    if (clus % 2 == 0) {
        fat[offset + 1] = (fat[offset + 1] & 0xf0) | ((newClus & 0xf00) >> 8);
        fat[offset] = newClus & 0x0ff;
    } else {
        fat[offset + 1] = ((newClus & 0x00f) << 4) | (fat[offset + 1] & 0x00f);
        fat[offset + 2] = (newClus & 0xff0) >> 4;
    }
}

void clearClus(unsigned short entClus) {
    for (unsigned short nextClus = getNextClus(entClus); nextClus != 0xfff;
         entClus = nextClus, nextClus = getNextClus(entClus))
        setFatClus(entClus, 0);
    setFatClus(entClus, 0);
}

const unsigned char *getFat() {
    return fat;
}
