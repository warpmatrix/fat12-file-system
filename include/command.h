#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>

#include "entry.h"
#include "io.h"

int dircmd(unsigned short curClus, const char *path,
           const unsigned char *ramFDD144);
int cdcmd(unsigned short *fstClus, const char *path,
          const unsigned char *ramFDD144);
int mdcmd(unsigned short clus, const char *path, unsigned char *ramFDD144);
void pwdcmd(unsigned short clus, const unsigned char *ramFDD144);
void clearcmd(void);

#endif
