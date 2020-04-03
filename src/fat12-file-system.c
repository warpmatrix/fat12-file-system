#include <stdio.h>
#include <string.h>

#include "command.h"
#include "io.h"
#include "shell.h"

#include "fat12-header.h"

const char FILENAME[] = "disk/disk.flp";

int init(unsigned char *ramFDD144);

int main(int argc, char const *argv[]) {
    unsigned char ramFDD144[SIZE];
    int res = init(ramFDD144);
    if (res == -1) {
        printf("Cannot find the disk\n");
        return 0;
    } else if (res == -2) {
        printf("File allocation table error\n");
    } else if (res >=0 && res < SIZE) {
        printf("File size(%d) doesn't match\n", res);
        return 0;
    } else {
        printf("Successfully Loaded!\n");
        printf("\n");
    }

    unsigned short clus = 0;
    printPath(clus, ramFDD144), printf("$ ");
    Command cmd = inputCmd();
    while (cmd.argc == 0 || cmd.argc > 0 && strcmp(cmd.argv[0], "quit")) {
        if (cmd.argc > 0)
            if (!strcmp(cmd.argv[0], "ls")) {
                int res = lscmd(clus, cmd.argv[1], ramFDD144);
                if (res == -1) printCmd(&cmd), printf(": No such directory\n");
            } else if (!strcmp(cmd.argv[0], "cd")) {
                int res = cdcmd(&clus, cmd.argv[1], ramFDD144);
                if (res == -1) printCmd(&cmd), printf(": No such directory\n");
            } else if (!strcmp(cmd.argv[0], "mkdir")) {
                int res = mkdircmd(clus, cmd.argv[1], ramFDD144);
                if (res == -1) printCmd(&cmd), printf(": Missing operand\n");
                else if (res == -2) printCmd(&cmd), printf(": No such directory\n");
                else if (res == -3) printCmd(&cmd), printf(": File or directory exists\n");
                else if (res == -4) printCmd(&cmd), printf(": Root directory is full\n");
                else if (res == -5) printCmd(&cmd), printf(": Disk is full\n");
            } else if (!strcmp(cmd.argv[0], "rmdir")) {
                int res = rmdircmd(clus, cmd.argv[1], ramFDD144);
            } else if (!strcmp(cmd.argv[0], "pwd")) {
                pwdcmd(clus, ramFDD144);
            } else if (!strcmp(cmd.argv[0], "clear")) {
                clearcmd();
            } else {
                printCmd(&cmd), printf(": command not found\n");
            }
        freeCmd(&cmd);
        printPath(clus, ramFDD144), printf("$ ");
        cmd = inputCmd();
    }
    freeCmd(&cmd);

    return 0;
}

int init(unsigned char *ramFDD144) {
    int cnt = Read_ramFDD(ramFDD144, FILENAME);
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
