#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vfs.h>

Vfs * vfs_create() {
    Vfs * res = kmalloc(1 * sizeof(Vfs));

    res->head = NULL;
    res->tail = NULL;

    res->filesystems_size = 10;
    res->filesystems_filled = 0;
    res->filesystems = kmalloc(res->filesystems_size * sizeof(FsIdentifier *));

    //    create_root(res);

    return res;
}

void vfs_free(Vfs * vfs) {
    Inode * curr = vfs->head;
    if (vfs->head == NULL) {
        // If no inode was ever made
        assert(vfs->tail == NULL);
    } else if (curr == vfs->tail) {
        if (curr->direntry != NULL) { free_direntry(curr->direntry); }

        curr->fs_identifier->operations->free_inode(curr);
    } else {
        while (curr != NULL) {
            if (curr->direntry != NULL) { free_direntry(curr->direntry); }

            Inode * last = curr;
            curr = curr->next;
            last->fs_identifier->operations->free_inode(last);
        }
    }

    kfree(vfs->filesystems);
    kfree(vfs);
}


VfsErr vfs_add_inode(Vfs * vfs, Inode * inode) {
    // inode is already present
    if (inode->next != NULL || vfs->tail == inode) { return ERR_EXISTS; }

    vfs->tail->next = inode;
    vfs->tail = inode;

    return OK;
}

VfsErr vfs_register(Vfs * vfs, const FsIdentifier * ident) {
    vfs->filesystems[vfs->filesystems_filled++] = ident;
    if (vfs->filesystems_filled >= vfs->filesystems_size) {
        vfs->filesystems_size *= 2;
        vfs->filesystems =
            krealloc(vfs->filesystems, vfs->filesystems_size * sizeof(FsIdentifier *));

        if (vfs->filesystems == NULL) { return ERR_ALLOC_FAILED; }
    }

    return OK;
}


DirEntry * vfs_get_root(Vfs * vfs) {
    Inode * curr = vfs->head;
    do {
        DirEntry * direntry = curr->direntry;

        if (qstr_eq_null_terminated(&direntry->name, "/")) { return direntry; }

        curr = curr->next;
    } while (curr != vfs->tail);

    return NULL;
}
