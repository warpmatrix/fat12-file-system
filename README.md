# My FAT File System

The project mainly using C to simulate FAT12 file system. And it also uses the gtest.

## To Do List

- [ ] restruct unsigned short entIdx parsePath(unsigned short *dirclus, const char *path, const char ramFDD144);
- [ ] modify cmd format
- [x] restruct findDirClus->findEntIdx
- [x] restruct system (add cmd parser)
- [x] make testing disk
- [x] validate file size result
- [x] findout the usage of the dir (whether it can use path parameter)

    it can be use as "dir \<path>".
