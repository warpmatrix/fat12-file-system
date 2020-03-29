#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>

#include "entry.h"
#include "io.h"

int listEnts(unsigned short fstClus, const char *path,
             const unsigned char *ramFDD144);
int changeDir(unsigned short *fstClus, const char *path,
              const unsigned char *ramFDD144);

#endif
