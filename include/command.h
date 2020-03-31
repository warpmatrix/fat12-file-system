#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>

#include "entry.h"
#include "io.h"

int dircmd(unsigned short curClus, const char *path,
           const unsigned char *ramFDD144);
int cdcmd(unsigned short *fstClus, const char *path,
          const unsigned char *ramFDD144);

#endif
