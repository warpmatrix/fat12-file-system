#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "dir-entry.h"
#include "editor.h"

#define CMDLEN 256

typedef struct Command {
    int argc;
    char *argv[CMDLEN / 2 + 1];
} Command;

Command inputCmd(void);
void initCmd(Command *cmd);
Command parseInp(const char *input);
void printCmd(const Command *cmd);
int excuteCmd(const Command *cmd, unsigned short *clus,
              unsigned char *ramFDD144);
void freeCmd(Command *cmd);

void printErrInfo(const Command *cmd, int res);

// ret entClus and modify dirClus
unsigned short parsePath(unsigned short *dirClus, const char *path,
                         const unsigned char *ramFDD144);
char *getPathEntname(const char *path, char *entname);

#endif
