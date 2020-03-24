#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include "entry.h"

int listEnts(unsigned short fstClus, const unsigned char *ramFDD144);
int changeDir(unsigned short *fstClus, const unsigned char *ramFDD144);

#endif
