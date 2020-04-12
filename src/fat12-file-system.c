#include <stdio.h>
#include <string.h>

#include "fat12-header.h"
#include "io.h"
#include "shell.h"

int init(unsigned char *ramFDD144, const char *filename);

int main(int argc, char const *argv[]) {
    unsigned char ramFDD144[SIZE];
    int res = init(ramFDD144, argv[1]);
    if (res != SIZE) {
        if (res == -1) printf("Cannot find the disk\n");
        if (res == -2) printf("File allocation table error\n");
        if (res >= 0 && res < SIZE)
            printf("File size(%d) doesn't match\n", res);
        return res;
    }
    printf("Successfully Loaded!\n");
    printf("\n");

    unsigned short clus = 0;
    printPath(clus, ramFDD144), printf("$ ");
    Command cmd = inputCmd();
    while (cmd.argc == 0 || cmd.argc > 0 && strcmp(cmd.argv[0], "quit")) {
        if (cmd.argc > 0) {
            int res = excuteCmd(&cmd, &clus, ramFDD144);
            if (res != 0) printErrInfo(&cmd, res);
        }
        freeCmd(&cmd);
        printPath(clus, ramFDD144), printf("$ ");
        cmd = inputCmd();
    }
    freeCmd(&cmd);

    writeFats(ramFDD144);
    Write_ramFDD(ramFDD144, argv[1]);
    return 0;
}

int init(unsigned char *ramFDD144, const char *filename) {
    int cnt = Read_ramFDD(ramFDD144, filename);
    if (cnt != SIZE) return cnt;

    printf("MBR info:\n");
    unsigned char block[BLOCKSIZE];
    Read_ramFDD_Block(ramFDD144, 0, block);
    Fat12Header mbr = parseMbr(block);
    printMbrInfo(mbr);
    printf("\n");

    int res = initFat(ramFDD144);
    if (res == -1) return -2;

    return cnt;
}
