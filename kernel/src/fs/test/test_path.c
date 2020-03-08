#include <test.h>
#include <path.h>
#include <tmpfs.h>
#include <stdlib.h>
#include <string.h>

TEST_CREATE(path_create_test, {
    Path * p  = path_from_string("test");

    ASSERT_EQ(p->length, 4);
    ASSERT_EQ(u8a_get(p, 0), 't');
    ASSERT_EQ(u8a_get(p, 1), 'e');
    ASSERT_EQ(u8a_get(p, 2), 's');
    ASSERT_EQ(u8a_get(p, 3), 't');

    path_free(p);
})

TEST_CREATE(path_equal_test_1, {
    Path * p1  = path_from_string("/a/b");
    Path * p2  = path_from_string("/a/b");
    Path * p3  = path_from_string("/a/b/c");

    ASSERT(path_contents_equal(p1, p2));
    ASSERT(!path_contents_equal(p1, p3));

    path_free(p1);
    path_free(p2);
    path_free(p3);
})

TEST_CREATE(path_parent_test_1, {
    Path * p1  = path_from_string("/a/b/c");
    Path * p2  = path_from_string("/a/b");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_parent_test_2, {
    Path * p1  = path_from_string("./a/b");
    Path * p2  = path_from_string("./a");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_parent_test_3, {
    Path * p1  = path_from_string(".");
    Path * p2  = path_from_string(".");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_parent_test_4, {
    Path * p1  = path_from_string("");
    Path * p2  = path_from_string(".");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);

})

TEST_CREATE(path_parent_test_5, {
    Path * p1  = path_from_string("");
    Path * p2  = path_from_string(".");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_parent_test_6, {
    Path * p1  = path_from_string("./test");
    Path * p2  = path_from_string(".");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_parent_test_7, {
    Path * p1  = path_from_string(".test");
    Path * p2  = path_from_string(".");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_parent_test_8, {
    Path * p1  = path_from_string("test");
    Path * p2  = path_from_string(".");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_parent_test_9, {
    Path * p1  = path_from_string("test");
    Path * p2  = path_from_string(".");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_parent_test_10, {
    Path * p1  = path_from_string("/test");
    Path * p2  = path_from_string("/");

    path_parent(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_is_absolute_test, {
    Path * p1  = path_from_string("/test");

    ASSERT(path_is_absolute(p1));
    path_free(p1);
})

TEST_CREATE(path_is_relative_test_1, {
    Path * p1  = path_from_string("./test");

    ASSERT(path_is_relative(p1));
    path_free(p1);
})

TEST_CREATE(path_is_relative_test_2, {
    Path * p1  = path_from_string("test");

    ASSERT(path_is_relative(p1));
    path_free(p1);
})

TEST_CREATE(path_clone_test, {
    Path * p1  = path_from_string("/test");
    Path * p2  = path_clone(p1);

    ASSERT(path_contents_equal(p1, p2));
    path_free(p1);
    path_free(p2);
})

TEST_CREATE(path_find_file_test, {
    Vfs * test_vfs = vfs_create();
    tmpfs_init(test_vfs);

    create_tmpfs_root(test_vfs);

    DirEntry * root = vfs_get_root(test_vfs);

    DirEntry * newfile = create_direntry(qstr_from_null_terminated_string("test"), root);

    VfsErr err = OK;
    root->inode->fs_identifier->operations->create_file(root, newfile, &err);
    ASSERT_EQ(err, OK);
    Path *p = path_from_string("/test");

    DirEntry *d = path_get_direntry(test_vfs, p, &err);
    ASSERT_EQ(err, OK);



    ASSERT(qstr_eq_null_terminated(&d->name, "test"));

    path_free(p);
    vfs_free(test_vfs);
})

TEST_CREATE(path_filename_test, {
    Path * p = path_from_string("/test/test.txt");
    path_filename(p);
    

    ASSERT_EQ(strncmp((char *)p->array, "test.txt", p->length), 0);
    path_free(p);

})

TEST_CREATE(path_filename_test_trailing, {
    Path * p = path_from_string("/test/test/");
    path_filename(p);


    ASSERT_EQ(strncmp((char *)p->array, "test", p->length), 0);
    path_free(p);
})

TEST_CREATE(path_filename_test_1, {
    Path * p = path_from_string("../test/test/.");
    path_filename(p);

    ASSERT_EQ(strncmp((char *)p->array, "test", p->length), 0);
    path_free(p);
})

TEST_CREATE(path_filename_test_2, {
    Path * p = path_from_string("foo.txt/.//");
    path_filename(p);

    ASSERT_EQ(strncmp((char *)p->array, "foo.txt", p->length), 0);
    path_free(p);
})

TEST_CREATE(path_filename_test_3, {
    Path * p = path_from_string("/");
    path_filename(p);
    ASSERT_EQ(p->length, 0);
    path_free(p);
})

TEST_CREATE(path_filename_test_4, {
    Path * p = path_from_string("asd/..");
    path_filename(p);
    ASSERT_EQ(p->length, 0);
    path_free(p);
})
