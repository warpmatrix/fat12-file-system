#include "shell.h"

Command inputCmd(void) {
    Command cmd;
    char input[CMDLEN];
    getCmdLine(input);
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

int excuteCmd(const Command *cmd, unsigned short *clus,
              unsigned char *ramFDD144) {
    int res = 0;
    if (!strcmp(cmd->argv[0], "ls")) {
        res = lscmd(*clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "cd")) {
        res = cdcmd(clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "mkdir")) {
        res = mkdircmd(*clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "rmdir")) {
        res = rmdircmd(*clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "touch")) {
        res = touchcmd(*clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "rm")) {
        res = rmcmd(*clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "cp")) {
        res = cpcmd(*clus, cmd->argv[1], cmd->argv[2], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "cat")) {
        res = catcmd(*clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "edit")) {
        res = editcmd(*clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "tree")) {
        res = treecmd(*clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "pwd")) {
        res = pwdcmd(*clus, cmd->argv[1], ramFDD144);
    } else if (!strcmp(cmd->argv[0], "clear")) {
        res = clearcmd();
    } else {
        res = CMD_NOT_FOUND;
    }
    return res;
}

void freeCmd(Command *cmd) {
    for (size_t i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
}

void printErrInfo(const Command *cmd, int res) {
    printCmd(cmd), printf(": ");
    switch (res) {
        case MISS_OPERAND:
            printf("Missing operand\n");
            break;
        case NO_FILE_DIR:
            printf("No such file or directory\n");
            break;
        case FILE_DIR_XST:
            printf("File or directory exist\n");
            break;
        case ROOT_IS_FULL:
            printf("Root directory is full\n");
            break;
        case DISK_IS_FULL:
            printf("Disk is full\n");
            break;
        case NOT_RM_PRES_DIR:
            printf("Can't remove present directory\n");
            break;
        case INVALID_ARGUMET:
            printf("Invalid argument\n");
            break;
        case IS_DIRECTORY:
            printf("Is directory\n");
            break;
        case DIR_NOT_EMPTY:
            printf("Directory not empty\n");
            break;
        case FILE_ERROR:
            printf("File error\n");
            break;
        case OPER_NOT_PERM:
            printf("Operation not permitted\n");
            break;
        case MISS_DEST_FILE:
            printf("Missing destination file\n");
            break;
        case CMD_NOT_FOUND:
            printf("Command not found\n");
            break;
        case DIR_HAS_DOT:
            printf("directory name has '.'\n");
            break;
        case SOURCE_IS_DIR:
            printf("Source is directory\n");
            break;
    }
}

unsigned short parsePath(unsigned short *dirClus, const char *path,
                         const unsigned char *ramFDD144) {
    const char delim[] = "/";
    char *pathCopy = strdup(path), *cpyPtr = pathCopy;
    Entry entry;
    if (path[0] == '/') {
        entry.DIR_FstClus = *dirClus = 0;
        strsep(&pathCopy, delim);
    }
    for (const char *entname = strsep(&pathCopy, delim); entname != NULL;
         entname = strsep(&pathCopy, delim)) {
        if (!pathCopy && strlen(entname) == 0) break;
        entry = getEntByName(entname, *dirClus, ramFDD144);
        if (pathCopy && strlen(pathCopy)) *dirClus = entry.DIR_FstClus;
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
