#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include <string.h>

#include "dir-entry.h"

#define CMDLEN 256

#define MISS_OPERAND -1
#define NO_FILEORDIR -2
#define FILEORDIR_XST -3
#define ROOT_IS_FULL -4
#define DISK_IS_FULL -5

typedef struct Command {
    int argc;
    char *argv[CMDLEN / 2 + 1];
} Command;

Command inputCmd(void);
void initCmd(Command *cmd);
Command parseInp(const char *input);
void printCmd(const Command *cmd);
int excuteCmd(const Command *cmd, unsigned short clus, unsigned char *ramFDD144);
void freeCmd(Command *cmd);

void printErrInfo(int res);

// ret entClus and modify dirClus
unsigned short parsePath(unsigned short *dirClus, const char *path,
                         const unsigned char *ramFDD144);
char *getPathEntname(const char *path, char *entname);

#endif
