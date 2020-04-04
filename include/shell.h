#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include <string.h>

#include "dir-entry.h"

#define CMDLEN 256

typedef struct Command {
    int argc;
    char *argv[CMDLEN / 2 + 1];
} Command;

Command inputCmd(void);
void initCmd(Command *cmd);
Command parseInp(const char *input);
void printCmd(const Command *cmd);
void freeCmd(Command *cmd);

// ret entClus and modify dirClus
unsigned short parsePath(unsigned short *dirClus, const char *path,
                         const unsigned char *ramFDD144);
size_t findPath(char (*path)[12], unsigned short entClus,
                const unsigned char *ramFDD144);
void printPath(unsigned short clus, const unsigned char *ramFDD144);

#endif
