
#ifndef TMPFS_H
#define TMPFS_H

#include <vfs.h>
#include <stdint.h>
#include <u8_array_list.h>
#include <vp_array_list.h>

const static int TMPFS_DEFAULT_FILE_ALLOC_SIZE = 16;
const static int TMPFS_DEFAULT_DIR_ALLOC_SIZE = 4;

typedef struct {
    Inode base;

    union {
        U8ArrayList * filedata; // if it's a file
        VPArrayList * direntries;  // if it's a directory
    } data;
} TmpfsInode;

// Initializes tmpfs and registers it to the vfs
VfsErr tmpfs_init(Vfs * vfs);

// Creates a tmpfs root dir on the vfs;
TmpfsInode * create_tmpfs_root(Vfs * vfs);

const FsOperations tmpfs_inode_ops;
const struct FsIdentifier * FS_TMPFS;
#endif