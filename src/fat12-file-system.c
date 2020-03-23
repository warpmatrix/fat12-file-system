#include <stdio.h>
#include <string.h>

#include "command.h"
#include "initial.h"

int main() {
    unsigned char ramFDD144[SIZE];
    if (!init(ramFDD144)) return 0;

    unsigned short clus = 0;
    char cmd[256];
    printf("cmd: ");
    while (scanf("%s", cmd) != EOF) {
        if (!strcmp(cmd, "dir"))
            listEnts(clus, ramFDD144);
        else if (!strcmp(cmd, "cd"))
            changeDir(&clus, ramFDD144);
        printf("cmd: ");
    }

    return 0;
}
