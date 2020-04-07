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

int excuteCmd(const Command *cmd, unsigned short clus, unsigned char *ramFDD144) {
    int res;
    if (!strcmp(cmd->argv[0], "ls")) {
        res = lscmd(clus, cmd->argv[1], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": No such directory\n");
        }
    } else if (!strcmp(cmd->argv[0], "cd")) {
        res = cdcmd(&clus, cmd->argv[1], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": No such directory\n");
        }
    } else if (!strcmp(cmd->argv[0], "mkdir")) {
        res = mkdircmd(clus, cmd->argv[1], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": Missing operand\n");
            else if (res == -2) printCmd(&cmd), printf(": No such directory\n");
            else if (res == -3) printCmd(&cmd), printf(": File or directory exists\n");
            else if (res == -4) printCmd(&cmd), printf(": Directory name contains '.'\n");
            else if (res == -5) printCmd(&cmd), printf(": Root directory is full\n");
            else if (res == -6) printCmd(&cmd), printf(": Disk is full\n");
        }
    } else if (!strcmp(cmd->argv[0], "rmdir")) {
        res = rmdircmd(clus, cmd->argv[1], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": Missing operand\n");
            else if (res == -2) printCmd(&cmd), printf(": No such directory\n");
            else if (res == -3) printCmd(&cmd), printf(": Can't remove present directory\n");
            else if (res == -4) printCmd(&cmd), printf(": Not a directory\n");
            else if (res == -5) printCmd(&cmd), printf(": Invalid argument\n");
            else if (res == -6) printCmd(&cmd), printf(": Directory not empty\n");
            else if (res == -7) printCmd(&cmd), printf(": Directory entry number error\n");;
        }
    } else if (!strcmp(cmd->argv[0], "touch")) {
        res = touchcmd(clus, cmd->argv[1], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": Missing operand\n");
            else if (res == -2) printCmd(&cmd), printf(": No such directory\n");
            else if (res == -3) printCmd(&cmd), printf(": File or directory exists\n");
        }
    } else if (!strcmp(cmd->argv[0], "rm")) {
        res = rmcmd(clus, cmd->argv[1], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": Missing operand\n");
            else if (res == -2) printCmd(&cmd), printf(": No such file\n");
            else if (res == -3) printCmd(&cmd), printf(": Is a directory\n");
            else if (res == -4) printCmd(&cmd), printf(": Operation not permitted\n");
        }
    } else if (!strcmp(cmd->argv[0], "cp")) {
        res = cpcmd(clus, cmd->argv[1], cmd->argv[2], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": Missing file operand\n");
            else if (res == -2) printCmd(&cmd), printf(": Missing destination file\n");
            else if (res == -3) printCmd(&cmd), printf(": No such file\n");
            else if (res == -4) printCmd(&cmd), printf(": No such destination directory\n");
            else if (res == -5) printCmd(&cmd), printf(": File or directory exist\n");
            else if (res == -6) printCmd(&cmd), printf(": Source is a directory\n");
            else if (res == -7) printCmd(&cmd), printf(": Root directory is full\n");
            else if (res == -8) printCmd(&cmd), printf(": Disk is full\n");
        }
    } else if (!strcmp(cmd->argv[0], "cat")) {
        res = catcmd(clus, cmd->argv[1], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": Missing operand\n");
            else if (res == -2) printCmd(&cmd), printf(": No such file\n");
            else if (res == -3) printCmd(&cmd), printf(": Is a directory\n");
            else if (res == -4) printCmd(&cmd), printf(": File error\n");
        }
    } else if (!strcmp(cmd->argv[0], "edit")) {
        res = editcmd(clus, cmd->argv[1], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": Missing operand\n");
            else if (res == -2) printCmd(&cmd), printf(": No such file\n");
            else if (res == -3) printCmd(&cmd), printf(": Is a directory\n");
            else if (res == -4) printCmd(&cmd), printf(": File error\n");
            else if (res == -5) printCmd(&cmd), printf(": Disk is full\n");
        }
    } else if (!strcmp(cmd->argv[0], "tree")) {
        res = treecmd(clus, cmd->argv[1], ramFDD144);
        if (res != 0) {
            if (res == -1) printCmd(&cmd), printf(": No such directory\n");
        }
    } else if (!strcmp(cmd->argv[0], "pwd")) {
        pwdcmd(clus, ramFDD144);
    } else if (!strcmp(cmd->argv[0], "clear")) {
        clearcmd();
    } else {
        printCmd(&cmd), printf(": command not found\n");
    }
    return res;
}

void freeCmd(Command *cmd) {
    for (size_t i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
}

void printErrInfo(int res) {}

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
