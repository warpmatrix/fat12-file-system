#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include <string.h>

#include "entry.h"

#define CMDLEN 256

typedef struct Command {
    int argc;
    char *argv[CMDLEN / 2 + 1];
} Command;

void inputCmd(Command *cmd, unsigned short clus, const unsigned char *ramFDD144);
void initCmd(Command *cmd);
void parseInp(const char *input, Command *cmd);
void printCmd(const Command *cmd);
void freeCmd(Command *cmd);

size_t findPath(char **path, unsigned short entClus, const unsigned char *ramFDD144);
void printPath(unsigned short clus, const unsigned char *ramFDD144);

#endif
