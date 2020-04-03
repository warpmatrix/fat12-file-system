#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>

#include "entry.h"
#include "shell.h"

int lscmd(unsigned short curClus, const char *path,
          const unsigned char *ramFDD144);
int cdcmd(unsigned short *fstClus, const char *path,
          const unsigned char *ramFDD144);
int mkdircmd(unsigned short clus, const char *path, unsigned char *ramFDD144);
void pwdcmd(unsigned short clus, const unsigned char *ramFDD144);
void clearcmd(void);

#endif
