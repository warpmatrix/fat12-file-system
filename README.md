# My FAT File System

The project mainly using C to simulate FAT12 file system. And it also uses google test for testing.

## To Do List

- [ ] add cmd: edit
- [ ] restruct: parsePath
- [ ] modify: cmd pwd
- [x] add cmd: rmdir, rm, touch, cat, cp
- [x] restruct: entry
- [x] add fat test
- [x] restruct: fat
- [x] add cmd: mkdir
- [x] modify cmd format
- [x] restruct system (add cmd parser)
- [x] make testing disk
- [x] validate file size result

## Structure

1. utils, io
2. entry, fat, fat12-header
3. dir-entry
4. shell, command
5. system
