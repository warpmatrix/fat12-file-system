# My FAT File System

The project mainly using C to simulate FAT12 file system. And it also uses google test for testing.

> The project is built in linux system, some api may not support in windows.

To run the project in linux, use commands:

```shell
make
./bin/my-fat12-file-system
```

Or, use ```make run-test``` to run google test.

## To Do List

- [ ] modify: cmd pwd
- [ ] add editor
- [x] modify: parsePath
- [x] add cmd: rmdir, rm, touch, cat, cp, edit, tree
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
2. entry, fat, fat12-header, dir-entry
3. operation
4. shell, command
5. system
