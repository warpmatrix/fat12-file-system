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
    addEntClus(dirClus, newNextClus);
    return 0;
}

int mkent(const char *entname, unsigned short dirClus,
          unsigned char *ramFDD144) {
    size_t blockIdx, entIdx;
    entIdx = getDirFreeEnt(&blockIdx, dirClus, ramFDD144);
    if (entIdx == (size_t)-1) return -1;
    if (entIdx == (size_t)-2) return -2;
    unsigned short newEntClus = getFreeClus();
    if (newEntClus == (unsigned short)-1) return -3;

    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, blockIdx, block);
    time_t wrtTime = time(NULL);
    Entry entry = getEnt(entname, FILE_ATTR, wrtTime, newEntClus, 0);
    parseEnt(&entry, block + entIdx * BYTSPERENT);
    Write_ramFDD_Block(block, ramFDD144, blockIdx);
    addEntClus(newEntClus, newEntClus);
    return 0;
}

bool markEntDel(unsigned short entClus, unsigned char *block) {
    for (size_t i = 0; i < 16; i++) {
        Entry entry = parseEntStr(block + i * BYTSPERENT);
        if (entry.DIR_FstClus == entClus) {
            block[i * BYTSPERENT] = 0xe5;
            return true;
        }
    }
    return false;
}

int rment(unsigned short entClus, unsigned short dirClus,
          unsigned char *ramFDD144) {
    if (dirClus == 0) {
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            if (markEntDel(entClus, block)) {
                Write_ramFDD_Block(block, ramFDD144, baseSec + secOfst);
                break;
            }
        }
    } else
        for (unsigned short clus = dirClus; clus != 0xfff;
             clus = getNextClus(clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, 31 + clus, block);
            if (markEntDel(entClus, block)) {
                Write_ramFDD_Block(block, ramFDD144, 31 + clus);
                break;
            }
        }
    clearClus(entClus);
}

int cpent(const Entry *entry, const char *destEntname,
          unsigned short destDirClus, unsigned char *ramFDD144) {
    size_t blockIdx, entIdx;
    entIdx = getDirFreeEnt(&blockIdx, destDirClus, ramFDD144);
    if (entIdx == (size_t)-1) return -1;
    if (entIdx == (size_t)-2) return -2;
    unsigned short destFstClus = getFreeClus();
    if (destFstClus == (unsigned short)-1) return -2;
    for (unsigned short clus = entry->DIR_FstClus; clus != 0xfff;
         clus = getNextClus(clus)) {
        unsigned short newClus = getFreeClus();
        if (newClus == (unsigned short)-1) {
            clearClus(destFstClus);
            return -2;
        }
        addEntClus(destFstClus, newClus);
    }

    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, blockIdx, block);
    time_t wrtTime = time(NULL);
    Entry destEnt = getEnt(destEntname, entry->DIR_Attr, wrtTime, destFstClus, entry->DIR_FileSize);
    parseEnt(&destEnt, block + entIdx * BYTSPERENT);
    Write_ramFDD_Block(block, ramFDD144, blockIdx);

    size_t bytsCnt = 0;
    for (unsigned short clus = entry->DIR_FstClus, destClus = destFstClus;
         clus != 0xfff;
         clus = getNextClus(clus), destClus = getNextClus(destClus)) {
        unsigned char block[BLOCKSIZE];
        Read_ramFDD_Block(ramFDD144, 31 + clus, block);
        unsigned char destBlock[BLOCKSIZE];
        Read_ramFDD_Block(ramFDD144, 31 + destClus, destBlock);
        for (size_t offset = 0; offset < BLOCKSIZE; offset++, bytsCnt++) {
            if (bytsCnt == entry->DIR_FileSize) {
                if (getNextClus(clus) != 0xfff) return -3;
                break;
            }
            destBlock[offset] = block[offset];
        }
        Write_ramFDD_Block(destBlock, ramFDD144, 31 + destClus);
    }
    return 0;
}

int mkdirent(const char *entname, unsigned short dirClus,
             unsigned char *ramFDD144) {
    if (strstr(entname, ".")) return -1;
    size_t blockIdx, entIdx;
    entIdx = getDirFreeEnt(&blockIdx, dirClus, ramFDD144);
    if (entIdx == (size_t)-1) return -2;
    if (entIdx == (size_t)-2) return -3;
    unsigned short newEntClus = getFreeClus();
    if (newEntClus == (unsigned short)-1) return -3;

    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, blockIdx, block);
    time_t wrtTime = time(NULL);
    Entry entry = getEnt(entname, DIR_ATTR, wrtTime, newEntClus, 0);
    parseEnt(&entry, block + entIdx * BYTSPERENT);
    Write_ramFDD_Block(block, ramFDD144, blockIdx);

    Entry entries[2];
    entries[0] = getEnt(".", DIR_ATTR, wrtTime, newEntClus, 0);
    entries[1] = getEnt("..", DIR_ATTR, wrtTime, dirClus, 0);
    Read_ramFDD_Block(ramFDD144, newEntClus + 31, block);
    parseEnts(entries, 2, block);
    Write_ramFDD_Block(block, ramFDD144, newEntClus + 31);
    addEntClus(newEntClus, newEntClus);
    return 0;
}

void printEnts(const Entry *entries, int entCnt) {
    for (size_t i = 0; i < entCnt; i++) {
        if (entries[i].DIR_Attr == HIDN_ATTR) continue;
        printf("DIR_Name: "), printStr(entries[i].DIR_Name, 11);
        printf(",    DIR_Attr: 0x%02X", entries[i].DIR_Attr);
        char time[20];
        parseWriTime(entries[i].DIR_WrtTime, entries[i].DIR_WrtDate, time);
        printf(",    WrtTime: %s", time);
        if (entries[i].DIR_Attr != DIR_ATTR)
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
