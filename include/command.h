#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include "entry.h"

void listEnts(unsigned short fstClus, const unsigned char *ramFDD144);
void changeDir(unsigned short *fstClus, const unsigned char *ramFDD144);

#endif
