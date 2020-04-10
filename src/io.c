#include "io.h"

int Read_ramFDD(unsigned char *ramFDD144, const char *diskname) {
    FILE *fp = fopen(diskname, "rb");
    if (fp == NULL) return -1;
    size_t cnt = 0;
    while (fscanf(fp, "%c", &ramFDD144[cnt]) != EOF) cnt++;
    fclose(fp);
    return cnt;
}

int Write_ramFDD(const unsigned char *ramFDD144, const char *diskname) {
    FILE *fp = fopen(diskname, "wb");
    if (fp == NULL) return -1;
    for (size_t i = 0; i < SIZE; i++)
        fprintf(fp, "%c", ramFDD144[i]);
    fclose(fp);
    return 0;
}

void Read_ramFDD_Block(const unsigned char *ramFDD144, int blockIdx,
                       unsigned char *block) {
    int base = blockIdx * BLOCKSIZE;
    for (size_t offset = 0; offset < BLOCKSIZE; offset++)
        block[offset] = ramFDD144[base + offset];
}

void Write_ramFDD_Block(const unsigned char *block, unsigned char *ramFDD144,
                        int blockIdx) {
    int base = blockIdx * BLOCKSIZE;
    for (size_t offset = 0; offset < BLOCKSIZE; offset++)
        ramFDD144[base + offset] = block[offset];
}
