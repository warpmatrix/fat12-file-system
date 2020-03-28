#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include <string.h>

const int CMDLEN = 256;

typedef struct Command {
    int argc;
    char *argv[CMDLEN / 2 + 1];
} Command;

void inputCmd(Command *cmd);
void initCmd(Command *cmd);
void parseInp(const char *input, Command *cmd);
void freeCmd(Command *cmd);

#endif
