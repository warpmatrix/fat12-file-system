#include "shell.h"

#include <stdio.h>

void inputCmd(Command *cmd) {
    char input[CMDLEN];
    printf("cmd: ");
    fgets(input, CMDLEN, stdin);
    parseInp(input, cmd);
}

void initCmd(Command *cmd) {
    cmd->argc = 0;
    for (size_t i = 0; i < CMDLEN / 2 + 1; i++)
        cmd->argv[i] = NULL;
}

void parseInp(const char *input, Command *cmd) {
    initCmd(cmd);
    char *inpCopy = strdup(input), *cpyPtr = inpCopy;
    char delim[] = " \n";
    for (char *ptr = strsep(&inpCopy, delim); ptr != NULL;
         ptr = strsep(&inpCopy, delim))
        if (*ptr != '\0') cmd->argv[cmd->argc++] = strdup(ptr);
    free(cpyPtr);
}

void freeCmd(Command *cmd) {
    for (size_t i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
}
