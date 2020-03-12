#include <inode.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tmpfs.h>
#include <u8_array_list.h>
#include <vfs.h>
#include <vp_array_list.h>

VfsErr tmpfs_init(Vfs * vfs) {
    return vfs_register(vfs, FS_TMPFS);
}

TmpfsInode * create_tmpfs_root(Vfs * vfs) {
    Qstr name = qstr_from_null_terminated_string("/");

    DirEntry * entry = create_direntry(name, NULL);


    Inode * root = (Inode *)kmalloc(1 * sizeof(TmpfsInode));

    *root = create_inode_base(vfs, DIRECTORY, FS_TMPFS, entry);

    TmpfsInode * t_root = (TmpfsInode *)root;

    t_root->data.direntries = vpa_create(TMPFS_DEFAULT_DIR_ALLOC_SIZE);

    entry->inode = root;

    // Set head/tail
    vfs->head = root;
    vfs->tail = root;
    root->next = NULL;

    return (TmpfsInode *)root;
}

void add_entry_to_directory(DirEntry * parent, DirEntry * child, enum VfsErr * err) {
    TmpfsInode * parent_inode = (TmpfsInode *)parent->inode;

    if (parent_inode->base.inode_type != DIRECTORY) {
        if (err != NULL && *err == OK) { *err = ERR_NOT_DIR; }
        return;
    }

    vpa_push(parent_inode->data.direntries, child);
    child->parent = parent;
}

Inode * tmpfs_create_dir(DirEntry * parent, DirEntry * entry, enum VfsErr * err) {
    Vfs * vfs = parent->inode->vfs;

    TmpfsInode * inode = kmalloc(1 * sizeof(TmpfsInode));

    if (inode == NULL) {
        if (*err != OK) { *err = ERR_ALLOC_FAILED; }
        return NULL;
    }

    *inode = (TmpfsInode){
        .base = create_inode_base(vfs, DIRECTORY, FS_TMPFS, entry),
        .data.direntries = vpa_create(TMPFS_DEFAULT_DIR_ALLOC_SIZE),
    };

    entry->inode = (Inode *)inode;

    add_entry_to_directory(parent, entry, err);
    if (*err != OK) { return NULL; }


    vfs_add_inode(vfs, (Inode *)inode);

    return (Inode *)inode;
}

Inode * tmpfs_create_file(DirEntry * parent, DirEntry * entry, enum VfsErr * err) {
    Vfs * vfs = parent->inode->vfs;

    TmpfsInode * inode = kmalloc(1 * sizeof(TmpfsInode));

    if (inode == NULL) {
        if (*err != OK) { *err = ERR_ALLOC_FAILED; }
        return NULL;
    }

    *inode = (TmpfsInode){
        .base = create_inode_base(vfs, FILE, FS_TMPFS, entry),
        .data.filedata = u8a_create(TMPFS_DEFAULT_FILE_ALLOC_SIZE),
    };

    entry->inode = (Inode *)inode;

    add_entry_to_directory(parent, entry, err);
    if (*err != OK) { return NULL; }

    vfs_add_inode(vfs, (Inode *)inode);

    return (Inode *)inode;
}

File * tmpfs_open_file(Inode * inode, enum VfsErr * err) {
    File * file = (File *)kmalloc(sizeof(File));
    if (file == NULL) {
        if (*err != OK) { *err = ERR_ALLOC_FAILED; }
        return NULL;
    }

    inode->refcount++;
    file->dentry = inode->direntry;
    file->operations = inode->fs_identifier->operations;
    file->file_position = 0;

    return file;
}


void tmpfs_close_file(File * file, enum VfsErr * err) {
    Inode * inode = file->dentry->inode;
    --inode->refcount;
    if (inode->should_delete && inode->refcount <= 0) {
        inode->fs_identifier->operations->remove_file(file->dentry, err);
    }

    kfree(file);
}

size_t tmpfs_read_file(File * fp, uint8_t * buf, size_t count, enum VfsErr * err) {
    TmpfsInode * inode = (TmpfsInode *)fp->dentry->inode;
    U8ArrayList * array = inode->data.filedata;

    uint8_t * bpos = array->array + fp->file_position;
    uint8_t * last = memcpy(buf, bpos, min(count, array->length - fp->file_position));

    size_t numb = last - buf;
    fp->file_position += numb;

    return numb;
}


size_t tmpfs_write_file(File * fp, uint8_t * buf, size_t count, enum VfsErr * err) {
    TmpfsInode * inode = (TmpfsInode *)fp->dentry->inode;
    U8ArrayList * array = inode->data.filedata;

    u8a_resize(array, array->capacity + count);

    memcpy(array->array + fp->file_position, buf, count);
    fp->file_position += count;

    // Set the length of the array to be as long as what was already in it plus what we wrote to it.
    array->length = fp->file_position;

    return count;
}

void tmpfs_remove_file(DirEntry * entry, enum VfsErr * err) {}

VPArrayList * tmpfs_list_dir(DirEntry * entry, enum VfsErr * err) {
    TmpfsInode * inode = (TmpfsInode *)entry->inode;
    return inode->data.direntries;
}

void tmpfs_free_inode(Inode * inode) {
    TmpfsInode * tmpfsInode = (TmpfsInode *)inode;
    if (tmpfsInode->base.inode_type == FILE) {
        u8a_free(tmpfsInode->data.filedata);
    } else if (tmpfsInode->base.inode_type == DIRECTORY) {
        vpa_free(tmpfsInode->data.direntries, NULL);
    }

    kfree(tmpfsInode);
}

const FsOperations tmpfs_fs_ops = {
    .create_file = tmpfs_create_file,
    .create_dir = tmpfs_create_dir,

    .remove_file = tmpfs_remove_file,
    .list_dir = tmpfs_list_dir,

    .free_inode = tmpfs_free_inode,

    .open = tmpfs_open_file,
    .close = tmpfs_close_file,
    .read = tmpfs_read_file,
    .write = tmpfs_write_file,
};

const FsIdentifier tmpfs_id = {
    .fsname = "tmpfs",
    .operations = &tmpfs_fs_ops,
};

const FsIdentifier * FS_TMPFS = &tmpfs_id;
