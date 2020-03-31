#include "command.h"

int dircmd(unsigned short curClus, const char *path,
           const unsigned char *ramFDD144) {
    unsigned short entClus = curClus, dirClus = curClus;
    if (path) entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short) -1) return -1;
    if (entClus) {
        Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
        if (entry.DIR_Attr != 0x10) return -1;
    }
    listEnts(entClus, ramFDD144);
    return 0;
}

int cdcmd(unsigned short *clus, const char *path,
          const unsigned char *ramFDD144) {
    unsigned short entClus = 0, dirClus = *clus;
    if (path) entClus = parsePath(&dirClus, path, ramFDD144);
    if (entClus == (unsigned short) -1) return -1;
    if (entClus) {
        Entry entry = getEntByClus(entClus, dirClus, ramFDD144);
        if (entry.DIR_Attr != 0x10) return -1;
    }
    *clus = entClus;
    return 0;
}

void clearcmd() { fputs("\x1b[2J\x1b[H", stdout); }
