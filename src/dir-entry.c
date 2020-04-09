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

size_t getFreeEntIdx(const unsigned char *block) {
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

size_t getEntIdx(unsigned short entClus, const unsigned char *block) {
    for (size_t i = 0; i < 16; i++) {
        Entry entry = parseEntStr(block + i * BYTSPERENT);
        if (entry.DIR_FstClus == entClus) return i;
    }
    return -1;
}

size_t getDirEnt(size_t *blockIdx, unsigned short entClus,
                 unsigned short dirClus, const unsigned char *ramFDD144) {
    if (dirClus == 0) {
        for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            size_t entIdx = getEntIdx(entClus, block);
            if (entIdx != (size_t)-1) {
                *blockIdx = baseSec + secOfst;
                return entIdx;
            }
        }
        return -1;  // not found
    }
    for (unsigned short clus = dirClus; clus != 0xfff;
         clus = getNextClus(clus)) {
        unsigned char block[BLOCKSIZE];
        Read_ramFDD_Block(ramFDD144, 31 + clus, block);
        size_t entIdx = getEntIdx(entClus, block);
        if (entIdx != (size_t)-1) {
            *blockIdx = 31 + clus;
            return entIdx;
        }
    }
    return -1;  // not found
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

size_t findPath(char (*path)[12], unsigned short entClus,
                const unsigned char *ramFDD144) {
    size_t cnt = 0;
    while (entClus) {
        Entry dirEnt = getEntByName("..", entClus, ramFDD144);
        Entry entry = getEntByClus(entClus, dirEnt.DIR_FstClus, ramFDD144);
        for (size_t i = 0; i < 11; i++) {
            if (entry.DIR_Name[i] == ' ') {
                path[cnt][i] = '\0';
                break;
            }
            path[cnt][i] = entry.DIR_Name[i];
            if (i == 10) path[cnt][11] = '\0';
        }
        cnt++;
        entClus = dirEnt.DIR_FstClus;
    }
    strcpy(path[cnt++], "/");
    return cnt;
}

void printPath(unsigned short clus, const unsigned char *ramFDD144) {
    char path[BLOCKNUM - 33][12];
    size_t cnt = findPath(path, clus, ramFDD144);
    printf("\x1b[34;1m");
    for (int i = cnt - 1; i >= 0; i--) {
        if (i == 0 || i == cnt - 1)
            printf("%s", path[i]);
        else
            printf("%s/", path[i]);
    }
    printf("\x1b[0m");
}

void printTreeLine(const Stack *lastEnt, const Entry *entry, bool isLastEnt) {
    for (size_t depth = 0; depth < lastEnt->size; depth++) {
        if (depth < lastEnt->size - 1) {
            if (*(bool *)getStackVal(lastEnt, depth))
                printf("    ");
            else
                printf("|   ");
        } else {
            if (*(bool *)getStackVal(lastEnt, depth))
                printf("`-- ");
            else
                printf("|-- ");
        }
    }
    for (size_t i = 0; i < 11; i++) printf("%c", entry->DIR_Name[i]);
    printf("\n");
}
