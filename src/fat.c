#include "fat.h"

unsigned short getNextClus(const unsigned char *ramFDD144,
                           unsigned short clus) {
    unsigned short fat1Clus = getFatClus(ramFDD144 + BLOCKSIZE, clus);
    return fat1Clus;
}

unsigned short getFatClus(const unsigned char *fat, unsigned short clus) {
    unsigned short offset = clus / 2 * 3;
    if (clus % 2 == 0)
        return ((fat[offset + 1] & 0x0f) << 8) | fat[offset];
    else
        return (fat[offset + 2] << 4) | ((fat[offset + 1] >> 4) & 0x0f);
}

unsigned short getFreeClus(const unsigned char *ramFDD144) {
    const unsigned char *fat = ramFDD144 + BLOCKSIZE;
    for (unsigned short clus = 2; clus < BLOCKNUM - 33; clus++)
        if (getFatClus(fat, clus) == 0) return clus;
    return -1;
}

void addEntClus(unsigned char *ramFDD144, unsigned short fstClus,
                unsigned short newClus) {
    unsigned char *fat1 = ramFDD144 + BLOCKSIZE;
    unsigned char *fat2 = ramFDD144 + 10 * BLOCKSIZE;
    unsigned char *fat = fat1;
    if (getFatClus(fat, fstClus) == 0) {
        setFatClus(fat1, fstClus, 0xfff);
        setFatClus(fat2, fstClus, 0xfff);
        return;
    }
    unsigned short endClus = fstClus;
    for (unsigned short nextClus = getNextClus(ramFDD144, fstClus);
         nextClus != 0xfff;
         endClus = nextClus, nextClus = getNextClus(ramFDD144, nextClus))
        ;
    setFatClus(fat1, newClus, 0xfff);
    setFatClus(fat1, endClus, newClus);
    setFatClus(fat2, newClus, 0xfff);
    setFatClus(fat2, endClus, newClus);
}

void setFatClus(unsigned char *fat, unsigned short clus, unsigned short newClus) {
    unsigned short offset = clus / 2 * 3;
    if (clus % 2 == 0) {
        fat[offset + 1] = (fat[offset + 1] & 0xf0) | ((newClus & 0xf00) >> 8);
        fat[offset] = newClus & 0x0ff;
    } else {
        fat[offset + 1] = ((newClus & 0x00f) << 4) | (fat[offset + 1] & 0x00f);
        fat[offset + 2] = newClus & 0xff0 >> 4;
    }
}
