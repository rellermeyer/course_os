#include <fs.h>
#include <inode.h>
#include <stdio.h>
#include <stdlib.h>
#include <vfs.h>
uint32_t cnt = 0;

Inode create_inode_base(Vfs * vfs,
                        InodeType type,
                        const struct FsIdentifier * fs,
                        DirEntry * dirEntry) {
    return (Inode){
        .refcount = 0,
        .id = ++cnt,
        .inode_type = type,
        .fs_identifier = fs,
        .direntry = dirEntry,
        .next = NULL,
        .should_delete = false,
        .vfs = vfs,
    };
}


DirEntry * create_direntry(Qstr name, DirEntry * parent) {
    DirEntry * entry = kmalloc(sizeof(DirEntry));

    entry->parent = parent;
    entry->name = name;

    return entry;
}

void free_direntry(DirEntry * entry) {
    qstr_free(&entry->name);
    kfree(entry);
}
