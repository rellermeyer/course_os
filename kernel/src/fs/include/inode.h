
#ifndef INODE_H
#define INODE_H

#include <fs.h>
#include <qstr.h>
#include <stdbool.h>
#include <stdint.h>


typedef enum {
    DIRECTORY,
    FILE,
} InodeType;

// This is the base Inode. Any filesystem can extend it for it's own needs.
// The first field in any extended Inode must be this Inode. This makes
// casting between extended inodes and base inodes seamless as long as you
// refer to inodes only by it's pointer.
typedef struct Inode {
    InodeType inode_type;
    uint32_t id;
    uint32_t refcount;
    const struct FsIdentifier * fs_identifier;
    struct DirEntry * direntry;
    struct Inode * next;
    bool should_delete;
    struct Vfs * vfs;
} Inode;

typedef struct DirEntry {
    struct Inode * inode;
    Qstr name;
    struct DirEntry * parent;
} DirEntry;

Inode create_inode_base(struct Vfs * vfs,
                        InodeType type,
                        const struct FsIdentifier * fs,
                        DirEntry * dirEntry);

DirEntry * create_direntry(Qstr name, DirEntry * parent);
void free_direntry(DirEntry * d);

#endif
