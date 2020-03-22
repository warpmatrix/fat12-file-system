#include "initial.h"

const int BLOCKNUM = 2880;
const int BLOCKSIZE = 512;
const int SIZE = 1474560;  // 2880 * 512

const char FILENAME[] = "disk.flp";

bool init(unsigned char *ramFDD144) {
    int cnt = Read_ramFDD(ramFDD144, FILENAME);
    if (cnt != SIZE) {
        printf("Cannot find the file or size error!\n");
        return false;
    } else
        printf("Successfully Loaded!\n");

    printf("MBR info:\n");
    Fat12Header mbr;
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 0, block);
    parseMbr(block, &mbr);
    printMbrInfo(mbr);
    printf("\n");
    return true;
}

int Read_ramFDD(unsigned char *ramFDD144, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) return -1;
    size_t cnt = 0;
    while (fscanf(fp, "%c", &ramFDD144[cnt]) != EOF) cnt++;
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

unsigned int parseNum(const unsigned char *block, size_t base, size_t len) {
    unsigned int num = 0;
    for (size_t offset = 0; offset < len; offset++) {
        num += block[base + offset] * (1 << (offset * 8));
    }
    return num;
}

void parseStr(const unsigned char *block, size_t base, size_t len, char *str) {
    for (size_t offset = 0; offset < len; offset++)
        str[offset] = block[base + offset];
}


void printBlock(const unsigned char *block) {
    const int ENTPERLINE = 32;
    for (size_t i = 0; i < BLOCKSIZE; i++) {
        if (i % ENTPERLINE == 0)
            printf("%02X", block[i]);
        else
            printf(" %02X", block[i]);
        if (i % 8 == 7) printf(" ");
        if (i % ENTPERLINE == ENTPERLINE - 1) printf("\n");
    }
}
