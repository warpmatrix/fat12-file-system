#include <stdio.h>
#include <string.h>
#include "fat12-header.h"
#include "initial.h"
#include "command.h"

int main() {
    unsigned char ramFDD144[SIZE];
    if (!init(&ramFDD144)) return 0;

    int clus = 0;
    char cmd[256];
    while (scanf("%s", cmd) != EOF) {
        if (!strcmp(cmd, "dir")) listEnts(clus, ramFDD144);
    }

    return 0;
}
