#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>

#include "operation.h"
#include "shell.h"

int lscmd(unsigned short clus, const char *path,
          const unsigned char *ramFDD144);
int cdcmd(unsigned short *clus, const char *path,
          const unsigned char *ramFDD144);
int mkdircmd(unsigned short clus, const char *path, unsigned char *ramFDD144);
int rmdircmd(unsigned short clus, const char *path, unsigned char *ramFDD144);
int touchcmd(unsigned short clus, const char *path, unsigned char *ramFDD144);
int rmcmd(unsigned short clus, const char *path, unsigned char *ramFDD144);
int cpcmd(unsigned short clus, const char *path, const char *destPath,
          unsigned char *ramFDD144);
int catcmd(unsigned short clus, const char *path,
           const unsigned char *ramFDD144);
int editcmd(unsigned short clus, const char *path, unsigned char *ramFDD144);
int treecmd(unsigned short clus, const char *path,
            const unsigned char *ramFDD144);
int pwdcmd(unsigned short clus, const unsigned char *path,
           const unsigned char *ramFDD144);
int clearcmd(void);

#endif
