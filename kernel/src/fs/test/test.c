#include <test.h>
#include <vfs.h>
#include <tmpfs.h>

TEST_CREATE(test_create_vfs, {
    Vfs * test_vfs = vfs_create();
    ASSERT_NOT_NULL(test_vfs);
    vfs_free(test_vfs);
})



TEST_CREATE(test_get_root, {
    Vfs * test_vfs = vfs_create();
    tmpfs_init(test_vfs);

    Inode * root1 = (Inode *)create_tmpfs_root(test_vfs);
    Inode * root2 = vfs_get_root(test_vfs)->inode;

    ASSERT_EQ(root1->id, root2->id);

    vfs_free(test_vfs);
})
