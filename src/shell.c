#include "shell.h"

#include <stdio.h>

void initCmd(Command *cmd) { cmd->argc = 0; }

void parseInp(const char *input, Command *cmd) {
    initCmd(cmd);
    char *inpCopy = strdup(input);
    char delim[] = " \n";
    for (char *ptr = strsep(&inpCopy, delim); ptr != NULL;
         ptr = strsep(&inpCopy, delim))
        if (*ptr != '\0') cmd->argv[cmd->argc++] = strdup(ptr);
    free(inpCopy);
}

void freeCmd(Command *cmd) {
    for (size_t i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
}
