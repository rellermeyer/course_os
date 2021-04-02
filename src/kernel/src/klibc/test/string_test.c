#include <test.h>
#include <string.h>
#include <stdlib.h>

TEST_CREATE(test_strcpy, {
        char buf[8];
        strcpy(buf, "hello");
        ASSERT_EQ(strcmp(buf, "hello"), 0);

        // Overwrite and make sure the NULL character
        // was copied too.
        char test[8] = "R&L";
        strcpy(test, "AD");
        ASSERT_EQ(strcmp(test, "AD"), 0);
    });

TEST_CREATE(test_strcat, {
        // Simple test
        char buf[12] = "course";
        strcat(buf, "_os");
        ASSERT_EQ(strcmp(buf, "course_os"), 0);

        // Empty src string
        char new[60] = "Operating Systems";
        strcat(new, "");
        ASSERT_EQ(strcmp(new, "Operating Systems"), 0);

        // Empty dst
        char empty[24];
        empty[0] = '\0';

        char *hello = "Hello world!";
        strcat(empty, hello);
        ASSERT_EQ(strcmp(empty, "Hello world!"), 0);

    });

TEST_CREATE(test_strncat, {
        char one[7] = "foo";
        char bar[4] = "bar";

        strncat(one, bar, 3);
        ASSERT_EQ(strcmp("foobar", one), 0);

        char two[8] = "foo";
        strncat(two, bar, 2);
        ASSERT_EQ(strcmp("fooba", two), 0);

        char three[8] = "foo";
        strncat(three, bar, 20);
    } );

TEST_CREATE(test_memcmp, {
        ASSERT_EQ(memcmp("a", "c", 1), -2);
        ASSERT_EQ(memcmp("c", "a", 1), 2);

        int five = 5;
        int eight = 8;
        ASSERT_EQ(memcmp(&five, &eight, 1), -3);
        ASSERT_EQ(memcmp(&eight, &five, 1), 3);

        // Reuse of variables
        five = 6;
        ASSERT_EQ(memcmp(&five, &eight, 1), -2);
        eight = 7;
        ASSERT_EQ(memcmp(&eight, &five, 1), 1);

        // If the one is longer it is larger
        ASSERT_GT(memcmp("zxf", "c", 3), 0);

        // Check for NULL
        ASSERT_GT(memcmp("a", "", 1), 0);
        ASSERT_LT(memcmp("", "a", 1), 0);
        ASSERT_EQ(memcmp("", "", 0), 0);

        // C99 compliance test
        ASSERT_EQ(memcmp("abc", "def", 0), 0);
    })

TEST_CREATE(test_memchr, {
        // Simple case
        char *one_char = "m";
        ASSERT_EQ(memchr(one_char, 'm', 1), one_char);
        ASSERT_EQ(memchr(one_char, 'n', 1), NULL);

        // It shouldn't find anything in a zero length
        ASSERT_EQ(memchr(one_char, 'm', 0), NULL);

        char *abc = "abc";
        ASSERT_EQ(memchr(abc, 'a', 2), abc);
        ASSERT_EQ(memchr(abc, 'b', 2), abc+1);
        ASSERT_EQ(memchr(abc, 'c', 2), NULL);

        // Duplicates
        char *uni = "Delft University of Technology";
        char *res = memchr(uni, 'e', 31);
        ASSERT_EQ(res, uni+1);
        res = memchr(res+1, 'e', 31);
        ASSERT_EQ(res, uni+10);

    })

TEST_CREATE(test_strchr, {
        // Simple case
        char *one_char = "m";
        ASSERT_EQ(strchr(one_char, 'm'), one_char);
        ASSERT_EQ(strchr(one_char, 'n'), NULL);

        char *abc = "abc";
        ASSERT_EQ(strchr(abc, 'a'), abc);
        ASSERT_EQ(strchr(abc, 'b'), abc+1);
        ASSERT_EQ(strchr(abc, 'c'), abc+2);

        // Duplicates
        char *uni = "Delft University of Technology";
        ASSERT_EQ(strchr(uni, 'U'), uni+6);
    })

TEST_CREATE(test_strrchr, {
        // Simple case
        char *one_char = "m";
        ASSERT_EQ(strrchr(one_char, 'm'), one_char);
        ASSERT_EQ(strrchr(one_char, 'n'), NULL);

        // Just out of reach
        char *abc = "abc";
        ASSERT_EQ(strrchr(abc, 'a'), abc);
        ASSERT_EQ(strrchr(abc, 'b'), abc+1);
        ASSERT_EQ(strrchr(abc, 'c'), abc+2);

        // Backwards
        char *uni = "Delft University of Technology";
        ASSERT_EQ(strrchr(uni, 'e'), uni+21);
    })

TEST_CREATE(test_strstr, {
        // One character
        char *one = "a";
        char *two = "a";
        ASSERT_EQ(strstr(one, two), one);

        // Simple
        char *abc = "abc";
        ASSERT_EQ(strstr(abc, "b"), abc+1);

        // Delft
        char *uni = "Delft University of Technology";
        ASSERT_EQ(strstr(uni, "University"), uni+6);

        // Not found
        // Delft
        ASSERT_EQ(strstr(uni, "Amsterdam"), NULL);

        // s1 empty
        ASSERT_EQ(strstr("", abc), NULL);

        // s2 empty
        ASSERT_EQ(strstr(abc, ""), abc);
    })


TEST_CREATE(test_strlen, {
        // Simple word
        ASSERT_EQ(strlen("course_os"), 9);

        // Empty
        ASSERT_EQ(strlen(""), 0);

        // Multiple words
        ASSERT_EQ(strlen("kind of pointless, but you never know..."), 40);

    });

TEST_CREATE(test_ktokenize, {
        string tokens_nv[1] = {":"};
        VPSinglyLinkedList *lst = ktokenize("key: value", tokens_nv, 1);
        ASSERT_EQ(strncmp(vpsll_get(lst, 0), "key", 3), 0);
        kprintf("%s\n", vpsll_get(lst, 1));
        ASSERT_EQ(strncmp(vpsll_get(lst, 1), " value", 5), 0);

        vpsll_free(lst, kfree);

        string token_conf[2];
        token_conf[0] = ": ";
        token_conf[1] = ", ";

        // Parse a very simple config file
        lst = ktokenize("name: val, course: os, uni: tud", token_conf, 2);
        kprintf("%s\n", vpsll_get(lst, 1));
        ASSERT_EQ(strncmp(vpsll_get(lst, 0), "name", 4), 0);
        ASSERT_EQ(strncmp(vpsll_get(lst, 1), "val", 3), 0);
        ASSERT_EQ(strncmp(vpsll_get(lst, 2), "course", 6), 0);
        ASSERT_EQ(strncmp(vpsll_get(lst, 3), "os", 2), 0);
        ASSERT_EQ(strncmp(vpsll_get(lst, 4), "uni", 3), 0);
        ASSERT_EQ(strncmp(vpsll_get(lst, 5), "tud", 3), 0);

        vpsll_free(lst, kfree);

        // No matches
        lst = ktokenize("2 + 4", tokens_nv, 1);
        ASSERT_EQ(strncmp(vpsll_get(lst, 0), "2 + 4", 5), 0);
        vpsll_free(lst, kfree);

        // Empty string
        lst = ktokenize("", tokens_nv, 1);
        ASSERT_EQ(strncmp(vpsll_get(lst, 0), "", 0), 0);
        vpsll_free(lst, kfree);
    })
