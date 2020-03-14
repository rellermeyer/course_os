#include <fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vp_array_list.h>


void create_file(Path * p, VfsErr * err) {}

void create_dir(Vfs * vfs, Path * p, VfsErr * err) {
    if (path_exists(vfs, p)) {
        if (err != NULL && *err == OK) { *err = ERR_EXISTS; }
        return;
    }

    Path * parent = u8a_clone(p);
    path_parent(parent);

    VfsErr ourErr = OK;
    DirEntry * parentDir = path_get_direntry(vfs, parent, &ourErr);

    // TODO: make a macro for error handling?
    if (ourErr != OK || parentDir == NULL) {
        if (err != NULL && *err == OK && ourErr != OK) {
            *err = ourErr;
        } else if (err != NULL) {
            *err = ERR_NO_PARENT;
        }
        return;
    }

    DirEntry * child = kmalloc(sizeof(DirEntry));
    UNUSED(child);
    //    memcpy(child->name, )

    //    parentDir->inode->fs_identifier->operations->create_dir(parentDir, , &ourErr);
}

// void remove_dir(Path * p, bool recurse, VfsErr * err);
// void remove_file(Path * p, VfsErr * err);
//
// VPArrayList * list_dir(Path * p, VfsErr * err);
//
// File * open_file(Path * p, VfsErr * err);
// size_t read_file(File * f, uint8_t * buf, size_t length, VfsErr * err);
// size_t write_file(File * f, uint8_t * buf, size_t length, VfsErr * err);
// void seek_file(File * f, size_t pos, VfsErr * err);
// void close_file(File * f, VfsErr * err);
