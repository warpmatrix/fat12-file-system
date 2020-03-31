#include "shell.h"

#include <stdio.h>

void inputCmd(Command *cmd, unsigned short clus,
              const unsigned char *ramFDD144) {
    printPath(clus, ramFDD144), printf("$ ");
    char input[CMDLEN];
    fgets(input, CMDLEN, stdin);
    parseInp(input, cmd);
}

void initCmd(Command *cmd) {
    cmd->argc = 0;
    for (size_t i = 0; i < CMDLEN / 2 + 1; i++) cmd->argv[i] = NULL;
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

size_t findPath(char **path, unsigned short entClus, const unsigned char *ramFDD144) {
    size_t cnt = 0;
    while (entClus) {
        Entry dirEnt = getEntByName("..", entClus, ramFDD144);
        Entry entry = getEntByClus(entClus, dirEnt.DIR_FstClus, ramFDD144);
        path[cnt] = malloc(12 * sizeof(*path[cnt]));
        for (size_t i = 0; i < 11; i++) {
            if (entry.DIR_Name[i] == ' ') {
                path[cnt][i] = '\0';
                break;
            }
            path[cnt][i] = entry.DIR_Name[i];
            if (i == 10) path[cnt][11] = '\0';
        }
        cnt++;
        entClus = dirEnt.DIR_FstClus;
    }
    return cnt;
}

void printPath(unsigned short clus, const unsigned char *ramFDD144) {
    char **path = malloc((BLOCKNUM - 33) * sizeof(*path));
    size_t cnt = findPath(path, clus, ramFDD144);
    printf("/");
    for (int i = cnt - 1; i >= 0; i--) {
        if (i == 0)
            printf("%s", path[i]);
        else
            printf("%s/", path[i]);
        free(path[i]);
    }
    free(path);
}
