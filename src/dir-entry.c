#include "dir-entry.h"

void parseEnts(const Entry *entries, size_t entCnt, unsigned char *block) {
    for (size_t i = 0; i < entCnt; i++)
        if (i < entCnt) parseEnt(&entries[i], block + BYTSPERENT * i);
    memset(block + BYTSPERENT * entCnt, 0, (16 - entCnt) * BYTSPERENT);
}

size_t parseEntBlock(const unsigned char *block, Entry *entries) {
    size_t entCnt = 0;
    for (size_t entIdx = 0; entIdx < BLOCKSIZE / BYTSPERENT; entIdx++) {
        if (block[entIdx * BYTSPERENT] == 0) break;
        if (block[entIdx * BYTSPERENT] == 0xe5) continue;
        entries[entCnt++] = parseEntStr(block + entIdx * BYTSPERENT);
    }
    return entCnt;
}

size_t getFreeEntIdx(unsigned char *block) {
    for (size_t i = 0; i < 16; i++) {
        unsigned char fstByte = block[i * BYTSPERENT];
        if (fstByte == 0xe5 || fstByte == 0) return i;
    }
    return -1;
}

size_t getDirFreeEnt(size_t *blockIdx, unsigned short dirClus,
                     unsigned char *ramFDD144) {
    if (dirClus == 0) {
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            size_t freeEntIdx = getFreeEntIdx(block);
            if (freeEntIdx != (size_t)-1) {
                *blockIdx = baseSec + secOfst;
                return freeEntIdx;
            }
        }
        return -1;
    }
    for (unsigned short clus = dirClus; clus != 0xfff;
         clus = getNextClus(clus)) {
        unsigned char block[BLOCKSIZE];
        Read_ramFDD_Block(ramFDD144, 31 + clus, block);
        size_t freeEntIdx = getFreeEntIdx(block);
        if (freeEntIdx != (size_t)-1) {
            *blockIdx = 31 + clus;
            return freeEntIdx;
        }
    }
    unsigned short newNextClus = getFreeClus();
    if (newNextClus == (unsigned short)-1) return -2;
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 31 + newNextClus, block);
    memset(block, 0, BLOCKSIZE);
    Write_ramFDD_Block(block, ramFDD144, 31 + newNextClus);
    *blockIdx = 31 + newNextClus;
    return 0;
}

int mkdirent(const char *entname, unsigned short dirClus,
             unsigned char *ramFDD144) {
    size_t blockIdx, entIdx;
    entIdx = getDirFreeEnt(&blockIdx, dirClus, ramFDD144);
    if (entIdx == (size_t)-1) return -1;
    if (entIdx == (size_t)-2) return -2;
    unsigned short newEntClus = getFreeClus();
    if (newEntClus == (unsigned short)-1) return -2;

    time_t wrtTime = time(NULL);
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, blockIdx, block);
    Entry entry = mknewEnt(entname, 0x10, wrtTime, newEntClus, 0);
    parseEnt(&entry, block + entIdx * BYTSPERENT);
    Write_ramFDD_Block(block, ramFDD144, blockIdx);

    Entry entries[2];
    entries[0] = mknewEnt(".", 0x10, wrtTime, newEntClus, 0);
    entries[1] = mknewEnt("..", 0x10, wrtTime, dirClus, 0);
    Read_ramFDD_Block(ramFDD144, newEntClus + 31, block);
    parseEnts(entries, 2, block);
    Write_ramFDD_Block(block, ramFDD144, newEntClus + 31);
    addEntClus(newEntClus, newEntClus);
    return 0;
}

void printEnts(const Entry *entries, int entCnt) {
    for (size_t i = 0; i < entCnt; i++) {
        if (entries[i].DIR_Attr == 0x27) continue;
        printf("DIR_Name: "), printStr(entries[i].DIR_Name, 11);
        printf(",    DIR_Attr: 0x%02X", entries[i].DIR_Attr);
        char time[20];
        parseWriTime(entries[i].DIR_WrtTime, entries[i].DIR_WrtDate, time);
        printf(",    WrtTime: %s", time);
        if (entries[i].DIR_Attr != 0x10)
            printf(",    FileSize: %d Bytes\n", entries[i].DIR_FileSize);
        else
            printf("\n");
    }
}

void listEnts(unsigned short fstClus, const unsigned char *ramFDD144) {
    if (fstClus == 0) {
        Entry entries[224];
        size_t entCnt = 0;
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            entCnt += parseEntBlock(block, entries + entCnt);
        }
        printEnts(entries, entCnt), printf("\n");
    } else {
        for (unsigned short clus = fstClus; clus != 0xfff;
             clus = getNextClus(clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, clus + 31, block);
            Entry entries[16];
            int entcnt = parseEntBlock(block, entries);
            printEnts(entries, entcnt), printf("\n");
        }
    }
}

Entry getEntByName(const char *entname, unsigned short dirClus,
                   const unsigned char *ramFDD144) {
    if (dirClus == 0) {
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            Entry entries[16];
            size_t entCnt = parseEntBlock(block, entries);
            for (size_t i = 0; i < entCnt; i++)
                if (entnameEq(entname, entries[i].DIR_Name)) return entries[i];
        }
        Entry entry = {.DIR_FstClus = -1};
        return entry;
    } else {
        for (unsigned short clus = dirClus; clus != 0xfff;
             clus = getNextClus(clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, clus + 31, block);
            Entry entries[16];
            size_t entCnt = parseEntBlock(block, entries);
            for (size_t i = 0; i < entCnt; i++)
                if (entnameEq(entname, entries[i].DIR_Name)) return entries[i];
        }
        Entry entry = {.DIR_FstClus = -1};
        return entry;
    }
}

Entry getEntByClus(unsigned short entClus, unsigned short dirClus,
                   const unsigned char *ramFDD144) {
    if (dirClus == 0) {
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            Entry entries[16];
            size_t entCnt = parseEntBlock(block, entries);
            for (size_t i = 0; i < entCnt; i++)
                if (entries[i].DIR_FstClus == entClus) return entries[i];
        }
        Entry entry = {.DIR_FstClus = -1};
        return entry;
    } else {
        for (unsigned short clus = dirClus; clus != 0xfff;
             clus = getNextClus(clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, clus + 31, block);
            Entry entries[16];
            size_t entCnt = parseEntBlock(block, entries);
            for (size_t i = 0; i < entCnt; i++)
                if (entries[i].DIR_FstClus == entClus) return entries[i];
        }
        Entry entry = {.DIR_FstClus = -1};
        return entry;
    }
}
