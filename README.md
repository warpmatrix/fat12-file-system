# My FAT File System

The project mainly using C to simulate FAT12 file system. And it also uses google test for testing.

## To Do List

- [ ] add cmd: rd, del, copy, type
- [ ] restruct: parsePath
- [ ] modify: cmd pwd
- [x] restruct: entry
- [x] add fat test
- [x] restruct: fat
- [x] add cmd: md
- [x] modify cmd format
- [x] restruct system (add cmd parser)
- [x] make testing disk
- [x] validate file size result

## Structure

1. utils, io, entry
2. dir-entry, fat, fat12-header
3. shell, command
4. system
