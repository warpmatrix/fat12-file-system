#include <stdio.h>
#include "fat12-header.h"
#include "entry.h"
#include "structrue.h"
#include "test.h"

// const char FILEPATH[] = "../";
const char FILENAME[] = "disk.flp";

int Read_ramFDD(unsigned char *ramFDD144, const char *filename);
void Read_ramFDD_Block(const unsigned char *ramFDD144, int blockIdx,
                       unsigned char *block);
void Write_ramFDD_Block(unsigned char *ramFDD144, int blockIdx,
                        const unsigned char *block);

int main() {
    unsigned char ramFDD144[SIZE];
    int cnt = Read_ramFDD(ramFDD144, FILENAME);
    if (cnt != SIZE) return 0;
    printf("\n");

    printf("MBR info:\n");
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 0, block);
    Fat12Header mbr;
    parseMbr(block, &mbr);
    printMbrInfo(mbr);
    printf("\n");

    printf("Root Entries:\n");
    Entry rootEntries[224];
    int entCnt = 0;
    for (size_t baseSec = 19, secOfst = 0; secOfst < 14; secOfst++) {
        Read_ramFDD_Block(ramFDD144, baseSec + secOfst, block);
        entCnt += parseEnts(block, rootEntries + entCnt);
    }
    printEnts(rootEntries, entCnt);

    return 0;
}

int Read_ramFDD(unsigned char *ramFDD144, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Cannot Find the File\n");
        return -1;
    }

    size_t cnt = 0;
    while (fscanf(fp, "%c", &ramFDD144[cnt]) != EOF) cnt++;
    if (cnt != SIZE)
        printf("Error\n");
    else
        printf("Successfully Loaded!\n");

    fclose(fp);
    return cnt;
}

void Read_ramFDD_Block(const unsigned char *ramFDD144, int blockIdx,
                       unsigned char *block) {
    int base = blockIdx * BLOCKSIZE;
    for (size_t offset = 0; offset < BLOCKSIZE; offset++)
        block[offset] = ramFDD144[base + offset];
}

void Write_ramFDD_Block(unsigned char *ramFDD144, int blockIdx,
                        const unsigned char *block) {
    int base = blockIdx * BLOCKSIZE;
    for (size_t offset = 0; offset < BLOCKSIZE; offset++)
        ramFDD144[base + offset] = block[offset];
}
