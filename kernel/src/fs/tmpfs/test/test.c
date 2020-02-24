#include <test.h>
#include <vfs.h>
#include <tmpfs.h>
#include <string.h>
#include <stdlib.h>

TEST_CREATE(test_create_file, {
    Vfs * test_vfs = vfs_create();
    tmpfs_init(test_vfs);

    TmpfsInode * root = (TmpfsInode *) create_tmpfs_root(test_vfs); // PARTIAL
    ASSERT_EQ(root->base.inode_type, DIRECTORY);

    DirEntry *newfile = create_direntry(qstr_from_null_terminated_string("test"), root->base.direntry);


    VfsErr err = OK;
    root->base.fs_identifier->operations->create_file(root->base.direntry, newfile, &err);
    ASSERT_EQ(err, OK);


    ASSERT_EQ(root->data.direntries->array[0], newfile);
    ASSERT_EQ(newfile->inode->inode_type, FILE);

    vfs_free(test_vfs);
})

TEST_CREATE(test_create_dir, {
    Vfs * test_vfs = vfs_create();
    tmpfs_init(test_vfs);

    TmpfsInode * root = (TmpfsInode *) create_tmpfs_root(test_vfs);
    ASSERT_EQ(root->base.inode_type, DIRECTORY);
    DirEntry *newdir = create_direntry(qstr_from_null_terminated_string("test"), root->base.direntry);

    VfsErr err = OK;
    root->base.fs_identifier->operations->create_dir(root->base.direntry, newdir, &err);

    ASSERT_EQ(err, OK);
    ASSERT_EQ(root->data.direntries->array[0], newdir);
    ASSERT_EQ(newdir->inode->inode_type, DIRECTORY);

    vfs_free(test_vfs);
})

TEST_CREATE(test_create_rw_file, {
    Vfs * test_vfs = vfs_create();
    tmpfs_init(test_vfs);

    TmpfsInode * root = (TmpfsInode *) create_tmpfs_root(test_vfs);
    ASSERT_EQ(root->base.inode_type, DIRECTORY);
    DirEntry *newfile = create_direntry(qstr_from_null_terminated_string("test"), root->base.direntry);

    VfsErr err = OK;
    root->base.fs_identifier->operations->create_file(root->base.direntry, newfile, &err);

    ASSERT_EQ(err, OK);
    ASSERT_EQ(root->data.direntries->array[0], newfile);
    ASSERT_EQ(newfile->inode->inode_type, FILE);

    File * file = newfile->inode->fs_identifier->operations->open(newfile->inode, &err);

    ASSERT_EQ(err, OK);
    ASSERT_EQ(file->file_position, 0);

    char * data = "The TMPFS works!";
    size_t length = strlen(data) + 1;
    size_t retlen1 = file->operations->write(file, (uint8_t *) data, length, &err);
    ASSERT_EQ(err, OK);
    ASSERT_EQ(length, retlen1);

    // Check if it didn't change the original string. Might be the same literal though
    // So might be always-true
    ASSERT_EQ(strcmp(data, "The TMPFS works!"), 0);

    // Reset the file pointer
    file->file_position = 0;

    char read_data[length];

    size_t retlen2 = file->operations->read(file, (uint8_t *)read_data, length, &err);
    ASSERT_EQ(err, OK);
    ASSERT_EQ(length, retlen2);
    ASSERT_EQ(strcmp(read_data, "The TMPFS works!"), 0);

    // Close file
    file->operations->close(file, &err);
    ASSERT_EQ(err, OK);

    vfs_free(test_vfs);
})

TEST_CREATE(test_listdir, {
    Vfs * test_vfs = vfs_create();
    tmpfs_init(test_vfs);

    create_tmpfs_root(test_vfs);

    DirEntry * root = vfs_get_root(test_vfs);

    DirEntry * newfile = create_direntry(qstr_from_null_terminated_string("test"), root);


    VfsErr err = OK;
    root->inode->fs_identifier->operations->create_file(root, newfile, &err);
    ASSERT_EQ(err, OK);
    VPArrayList *list = root->inode->fs_identifier->operations->list_dir(root, &err);
    ASSERT_EQ(err, OK);

    DirEntry * first = vpa_get(list, 0);

    ASSERT(qstr_eq_null_terminated(&first->name, "test"));

    vfs_free(test_vfs);
})