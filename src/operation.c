#include "operation.h"

int dispFile(const Entry *fileEnt, const unsigned char *ramFDD144) {
    size_t bytsCnt = 0;
    for (unsigned short clus = fileEnt->DIR_FstClus; clus != 0xfff;
         clus = getNextClus(clus)) {
        unsigned char block[BLOCKSIZE];
        Read_ramFDD_Block(ramFDD144, 31 + clus, block);
        for (size_t offset = 0; offset < BLOCKSIZE; offset++, bytsCnt++) {
            if (bytsCnt == fileEnt->DIR_FileSize) {
                if (getNextClus(clus) != 0xfff) return -1;
                break;
            }
            printf("%c", block[offset]);
        }
    }
    return 0;
}

int editFile(Entry *fileEnt, unsigned short dirClus, unsigned char *ramFDD144) {
    int res = dispFile(fileEnt, ramFDD144);
    if (res == -1) return -1;
    unsigned short clus = fileEnt->DIR_FstClus;
    while (getNextClus(clus) != 0xfff) clus = getNextClus(clus);
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 31 + clus, block);
    int offset = fileEnt->DIR_FileSize % BLOCKSIZE;

    const size_t LINELEN = 16;
    char line[LINELEN];
    while (fgets(line, LINELEN, stdin) != NULL) {
        size_t len = strlen(line);
        for (size_t i = 0; i < len; i++) {
            block[i + offset] = line[i];
            if (i + offset == BLOCKSIZE - 1) {
                Write_ramFDD_Block(block, ramFDD144, 31 + clus);
                unsigned short newClus = getFreeClus();
                if (newClus == (unsigned short)-1) return -2;
                addEntClus(fileEnt->DIR_FstClus, newClus);
                clus = newClus;
                Read_ramFDD_Block(ramFDD144, 31 + clus, block);
                offset -= BLOCKSIZE;
            }
        }
        fileEnt->DIR_FileSize += len;
        offset = fileEnt->DIR_FileSize % BLOCKSIZE;
    }
    Write_ramFDD_Block(block, ramFDD144, 31 + clus);

    size_t blockIdx, entIdx;
    entIdx = getDirEnt(&blockIdx, fileEnt->DIR_FstClus, dirClus, ramFDD144);
    if (entIdx == (size_t)-1) return -1;
    Read_ramFDD_Block(ramFDD144, blockIdx, block);
    parseTime(time(NULL), &fileEnt->DIR_WrtTime, &fileEnt->DIR_WrtDate);
    parseEnt(fileEnt, block + entIdx * BYTSPERENT);
    Write_ramFDD_Block(block, ramFDD144, blockIdx);
    return 0;
}

int mkent(const char *entname, unsigned short dirClus,
          unsigned char *ramFDD144) {
    size_t blockIdx, entIdx;
    entIdx = getDirFreeEnt(&blockIdx, dirClus, ramFDD144);
    if (entIdx == (size_t)-1) return ROOT_IS_FULL;
    if (entIdx == (size_t)-2) return DISK_IS_FULL;
    unsigned short newEntClus = getFreeClus();
    if (newEntClus == (unsigned short)-1) return DISK_IS_FULL;

    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, blockIdx, block);
    time_t wrtTime = time(NULL);
    Entry entry = getEnt(entname, FILE_ATTR, wrtTime, newEntClus, 0);
    parseEnt(&entry, block + entIdx * BYTSPERENT);
    Write_ramFDD_Block(block, ramFDD144, blockIdx);
    addEntClus(newEntClus, newEntClus);
    return 0;
}

int mkdirent(const char *entname, unsigned short dirClus,
             unsigned char *ramFDD144) {
    size_t blockIdx, entIdx;
    entIdx = getDirFreeEnt(&blockIdx, dirClus, ramFDD144);
    if (entIdx == (size_t)-1) return ROOT_IS_FULL;
    if (entIdx == (size_t)-2) return DISK_IS_FULL;
    unsigned short newEntClus = getFreeClus();
    if (newEntClus == (unsigned short)-1) return DISK_IS_FULL;

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
    Entry destEnt = getEnt(destEntname, entry->DIR_Attr, wrtTime, destFstClus,
                           entry->DIR_FileSize);
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

void printEnts(const Entry *entries, int entCnt) {
    for (size_t i = 0; i < entCnt; i++) {
        if (entries[i].DIR_Attr == HIDN_ATTR) continue;
        printEntInfo(&entries[i]);
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

void printTree(unsigned short dirClus, Stack *lastEnt,
               const unsigned char *ramFDD144) {
    static bool falseVal = false, trueVal = true;
    pushStack(&falseVal, lastEnt);
    if (dirClus == 0x000) {
        unsigned short baseSec = 19;
        Entry entries[224];
        size_t entCnt = 0;
        for (unsigned short secOfst = 0; secOfst < 14; secOfst++) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
            entCnt += parseEntBlock(block, entries + entCnt);
        }
        for (size_t i = 0; i < entCnt; i++) {
            bool isLastEnt = i == entCnt - 1;
            if (isLastEnt) setStackTopVal(lastEnt, &trueVal);
            printTreeLine(lastEnt, &entries[i], isLastEnt);
            if (entries[i].DIR_Attr == DIR_ATTR)
                printTree(entries[i].DIR_FstClus, lastEnt, ramFDD144);
            if (isLastEnt) popStack(lastEnt);
        }
    } else {
        for (unsigned short clus = dirClus; clus != 0xfff;
             clus = getNextClus(clus)) {
            unsigned char block[BLOCKSIZE];
            Read_ramFDD_Block(ramFDD144, 31 + clus, block);
            Entry entries[16];
            size_t entCnt = parseEntBlock(block, entries);
            for (size_t i = 0; i < entCnt; i++) {
                bool isLastEnt =
                    (i == entCnt - 1) && getNextClus(clus) == 0xfff;
                if (isLastEnt) setStackTopVal(lastEnt, &trueVal);
                if (!(entnameEq(".", entries[i].DIR_Name) ||
                      entnameEq("..", entries[i].DIR_Name))) {
                    printTreeLine(lastEnt, &entries[i], isLastEnt);
                    if (entries[i].DIR_Attr == DIR_ATTR)
                        printTree(entries[i].DIR_FstClus, lastEnt, ramFDD144);
                }
                if (isLastEnt) popStack(lastEnt);
            }
        }
    }
}
