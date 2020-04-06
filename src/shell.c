#include "shell.h"

#include <stdio.h>

Command inputCmd(void) {
    Command cmd;
    char input[CMDLEN];
    fgets(input, CMDLEN, stdin);
    cmd = parseInp(input);
    return cmd;
}

void initCmd(Command *cmd) {
    cmd->argc = 0;
    for (size_t i = 0; i < CMDLEN / 2 + 1; i++) cmd->argv[i] = NULL;
}

Command parseInp(const char *input) {
    Command cmd;
    initCmd(&cmd);
    char *inpCopy = strdup(input), *cpyPtr = inpCopy;
    char delim[] = " \n";
    for (char *ptr = strsep(&inpCopy, delim); ptr != NULL;
         ptr = strsep(&inpCopy, delim))
        if (*ptr != '\0') cmd.argv[cmd.argc++] = strdup(ptr);
    free(cpyPtr);
    return cmd;
}

void printCmd(const Command *cmd) {
    for (size_t i = 0; i < cmd->argc; i++)
        if (i == 0)
            printf("%s", cmd->argv[i]);
        else
            printf(" %s", cmd->argv[i]);
}

void freeCmd(Command *cmd) {
    for (size_t i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
}

unsigned short parsePath(unsigned short *dirClus, const char *path,
                         const unsigned char *ramFDD144) {
    const char delim[] = "/";
    char *pathCopy = strdup(path), *cpyPtr = pathCopy;
    if (path[0] == '/') {
        *dirClus = 0;
        strsep(&pathCopy, delim);
    }
    Entry entry;
    for (const char *entname = strsep(&pathCopy, delim); entname != NULL;
         entname = strsep(&pathCopy, delim)) {
        entry = getEntByName(entname, *dirClus, ramFDD144);
        if (pathCopy) *dirClus = entry.DIR_FstClus;
        if (entry.DIR_FstClus == (unsigned short)-1) break;
    }
    free(cpyPtr);
    return entry.DIR_FstClus;
}

char *getPathEntname(const char *path, char *entname) {
    char delim[] = "/";
    char *pathCopy = strdup(path), *cpyPtr = pathCopy;
    const char *partPath = strsep(&pathCopy, delim);
    while (pathCopy) partPath = strsep(&pathCopy, delim);
    strcpy(entname, partPath);
    free(cpyPtr);
    return entname;
}
