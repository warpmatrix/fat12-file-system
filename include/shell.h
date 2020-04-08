#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include <string.h>

#include "dir-entry.h"
#include "command.h"

#define CMDLEN 256

#define MISS_OPERAND 1
#define NO_FILE_DIR 2
#define FILE_DIR_XST 3
#define ROOT_IS_FULL 4
#define DISK_IS_FULL 5
#define NOT_RM_PRES_DIR 6
#define INVALID_ARGUMET 7
#define IS_DIRECTORY 8
#define DIR_NOT_EMPTY 9
#define FILE_ERROR 10
#define OPER_NOT_PERM 11
#define MISS_DEST_FILE 12
#define CMD_NOT_FOUND 13

#define DIR_HAS_DOT -2
#define SOURCE_IS_DIR -3

typedef struct Command {
    int argc;
    char *argv[CMDLEN / 2 + 1];
} Command;

Command inputCmd(void);
void initCmd(Command *cmd);
Command parseInp(const char *input);
void printCmd(const Command *cmd);
int excuteCmd(const Command *cmd, unsigned short *clus, unsigned char *ramFDD144);
void freeCmd(Command *cmd);

void printErrInfo(const Command *cmd, int res);

// ret entClus and modify dirClus
unsigned short parsePath(unsigned short *dirClus, const char *path,
                         const unsigned char *ramFDD144);
char *getPathEntname(const char *path, char *entname);

#endif
