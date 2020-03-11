#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <fs.h>
#include <vp_array_list.h>

struct InodeOperations;
struct inode;

typedef struct FsOperations {

    /// Inode ops
    // allocates a new Inode and returns this newly made Inode.
    struct Inode * (*create_file) (struct DirEntry * parent, struct DirEntry * child, enum VfsErr * err);
    struct Inode * (*create_dir) (struct DirEntry * parent, struct DirEntry * child, enum VfsErr * err);
    void (*remove_dir) (struct DirEntry * entry, enum VfsErr * err);
    VPArrayList * (*list_dir) (struct DirEntry * entry, enum VfsErr * err);
    void (*move) (struct Inode * oldDir, struct DirEntry * oldDirEntry, struct Inode * newDir, struct DirEntry * newDirEntry, enum VfsErr * err);
    void (*remove_file) (struct DirEntry * entry, enum VfsErr * err);

    void (*free_inode) (struct Inode * inode);


    /// File ops
    File * (*open)(struct Inode * inode, enum VfsErr * err);
    size_t (*read)(File * fp, uint8_t * buf, size_t count, enum VfsErr * err);
    size_t (*write)(File * fp, uint8_t * buf, size_t count, enum VfsErr * err);
    void (*close)(File * fp, enum VfsErr * err);

} FsOperations;

typedef struct FsIdentifier {
    char * fsname;
    const FsOperations * operations;
} FsIdentifier;

typedef struct Vfs {
    uint32_t filesystems_size;
    uint32_t filesystems_filled;
    const FsIdentifier ** filesystems; // TODO: change to vpa?

    // Inodes start at the head and are added at the tail end.
    // The linked list starts at head.
    struct Inode * head;
    struct Inode * tail;
} Vfs;

typedef enum VfsErr{
    OK,
    ERR_NOT_DIR,
    ERR_EXISTS,
    ERR_ALLOC_FAILED,
    ERR_NO_PARENT,
} VfsErr;

// Create and returns a VFS instance
Vfs * vfs_create();
// Adds an inode to the internal vfs inode list
VfsErr vfs_add_inode(Vfs * vfs, struct Inode * node);
// Register a new filesystem against the vfs
VfsErr vfs_register(Vfs * vfs, const FsIdentifier * fs_identifier);
// Frees all vfs related constructs: inodes,dentries, etc.
void vfs_free(Vfs * vfs);
// Get the direntry for the root of the filesystem: '/'
struct DirEntry * vfs_get_root(Vfs * vfs);

#endif
