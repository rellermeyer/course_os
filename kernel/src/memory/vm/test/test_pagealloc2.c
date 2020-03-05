#include <test.h>
#include <pagealloc2.h>
#include <string.h>

size_t listlength(struct MemorySliceInfo * start) {
    if (start == NULL) {
        return 0;
    }

    size_t count = 1;
    for (; start->next != NULL; start = start->next) {
        count++;
    }

    return count;
}

TEST_CREATE(test_memoryinfo_size, {
    ASSERT_EQ(sizeof(struct MemorySliceInfo), 16);
})

TEST_CREATE(test_empty, {
    ASSERT_NOT_NULL(pageallocator.unused);
    ASSERT_EQ(listlength(pageallocator.unused),1278);
    ASSERT_EQ(listlength(pageallocator.allocated), 2);
    ASSERT_NULL(pageallocator.l2ptPartialFree);
    ASSERT_NULL(pageallocator.pagePartialFree);
})

TEST_CREATE(test_allocate_pt, {
    struct L1PageTable * pt = pagealloc_allocate_l1_pagetable();

    // Just try to overwrite everything a few times so if something breaks we'll hopefully see in the next tests.
    memset(pt, 0, 1024 * 16);
    memset(pt, 1, 1024 * 16);


    ASSERT_NOT_NULL(pageallocator.unused);
    ASSERT_EQ(listlength(pageallocator.unused), 1277);
    ASSERT_NOT_NULL(pageallocator.allocated);
    ASSERT_EQ(listlength(pageallocator.allocated), 3);
    ASSERT_NULL(pageallocator.l2ptPartialFree);
    ASSERT_NULL(pageallocator.pagePartialFree);

    pagealloc_free_l1_pagetable(pt);

    ASSERT_EQ(listlength(pageallocator.unused), 1278);
    ASSERT_EQ(listlength(pageallocator.allocated), 2);
})

TEST_CREATE(test_allocate_many_pt, {
    int total = listlength(pageallocator.unused);
    int totalallocated = listlength(pageallocator.allocated);

    struct L1PageTable * pages[700];

    for (int i = 0; i < 700; i++) {

        pages[i] = pagealloc_allocate_l1_pagetable();
        ASSERT_NOT_NULL(pages[i]);
        ASSERT_EQ(listlength(pageallocator.unused), total - (i + 1u));
        ASSERT_EQ(listlength(pageallocator.allocated), totalallocated + i + 1);
    }

    struct L1PageTable * pt = pagealloc_allocate_l1_pagetable();

    ASSERT_NOT_NULL(pageallocator.unused);
    ASSERT_EQ(listlength(pageallocator.unused), 577);
    ASSERT_NOT_NULL(pageallocator.allocated);

    ASSERT_EQ(listlength(pageallocator.allocated), 703);
    ASSERT_NULL(pageallocator.l2ptPartialFree);
    ASSERT_NULL(pageallocator.pagePartialFree);

    pagealloc_free_l1_pagetable(pt);

    for (int i = 0; i < 700; i++) {
        pagealloc_free_l1_pagetable(pages[i]);
    }


    ASSERT_NOT_NULL(pageallocator.unused);
    ASSERT_EQ(listlength(pageallocator.unused),1278);
    ASSERT_EQ(listlength(pageallocator.allocated), 2);
    ASSERT_NULL(pageallocator.l2ptPartialFree);
    ASSERT_NULL(pageallocator.pagePartialFree);
})

// Test the magic :sparkles:
TEST_CREATE(test_get_sliceinfo, {
    struct L1PageTable * pt = pagealloc_allocate_l1_pagetable();

    struct MemorySliceInfo * info = pagealloc_get_sliceinfo_for_slice((union MemorySlice *)pt);

    ASSERT_EQ(&info->slice->l1pt, pt);

    pagealloc_free_l1_pagetable(pt);
})

TEST_CREATE(test_doubly_linked_sliceinfo, {
    struct MemorySliceInfo * curr = pageallocator.unused;
    uint32_t length = listlength(curr);

    uint32_t i = 0;
    do {
        i++;
        ASSERT_EQ(curr->next->prev, curr);
        curr = curr->next;
    }   while(curr->next != NULL);

    ASSERT_EQ(i, length-1);
})

TEST_CREATE(test_allocate_l2pt, {
    // Save how many slices are allocated now
    size_t totalallocated = listlength(pageallocator.allocated);

    // the l2ptPartialFree should start empty
    ASSERT_NULL(pageallocator.l2ptPartialFree);

    struct L2PageTable * pt1 = pagealloc_allocate_l2_pagetable();
    // After one allocation, it shouldn't be empty anymore
    ASSERT_NOT_NULL(pageallocator.l2ptPartialFree);
    // But it's length should be only one
    ASSERT_NULL(pageallocator.l2ptPartialFree->next);
    // The #allocated should have stayed the same as this node goes on l2ptPartialFree
    ASSERT_EQ(listlength(pageallocator.allocated), totalallocated);

    struct L2PageTable * pages[15];

    for (int i = 0; i < 14; i++) {
        pages[i] = pagealloc_allocate_l2_pagetable();
        ASSERT_NULL(pageallocator.l2ptPartialFree->next);
    }
    pages[14] = pagealloc_allocate_l2_pagetable();

    ASSERT_NULL(pageallocator.l2ptPartialFree);

    ASSERT_EQ(totalallocated + 1u, listlength(pageallocator.allocated));

    // Now as we allocate a 17th l1 pagetable, the #allocated should have grown by one
    struct L2PageTable * pt17 = pagealloc_allocate_l2_pagetable();
    ASSERT_EQ(listlength(pageallocator.allocated), totalallocated + 1u);
    // but l2ptPartialFree should now be of length one again
    ASSERT_NOT_NULL(pageallocator.l2ptPartialFree);
    ASSERT_NULL(pageallocator.l2ptPartialFree->next);

    ASSERT_EQ(pt1 + 1, pages[0]);

    // Assert pt2 comes after pt1 etc.
    for (int i = 0; i < 14; i++) {
        ASSERT_EQ(pages[i] + 1, pages[i+1]);
    }

    pagealloc_free_l2_pagetable(pt17);

    for (int i = 0; i < 15; i++) {
        pagealloc_free_l2_pagetable(pages[i]);
    }

    pagealloc_free_l2_pagetable(pt1);


    ASSERT_NOT_NULL(pageallocator.unused);
    ASSERT_EQ(listlength(pageallocator.unused),1278);
    ASSERT_EQ(listlength(pageallocator.allocated), 2);
    ASSERT_NULL(pageallocator.l2ptPartialFree);
    ASSERT_NULL(pageallocator.pagePartialFree);


    for (int i = 0; i < 100; i++) {
        struct L2PageTable * pages2[100];

        // Allocate 100 l2pts
        for (int i = 0; i < 100; i++) {
            pages2[i] = pagealloc_allocate_l2_pagetable();
        }

        // Now free them in a very different order
        for (int i = 0; i < 50; i++) {
            pagealloc_free_l2_pagetable(pages2[i]);
            pagealloc_free_l2_pagetable(pages2[99-i]);
        }

        ASSERT_NOT_NULL(pageallocator.unused);
        ASSERT_EQ(listlength(pageallocator.unused),1278);
        ASSERT_EQ(listlength(pageallocator.allocated), 2);
        ASSERT_NULL(pageallocator.l2ptPartialFree);
        ASSERT_NULL(pageallocator.pagePartialFree);
    }
})

TEST_CREATE(test_allocate_page, {
    // Save how many slices are allocated now
    size_t totalallocated = listlength(pageallocator.allocated);

    // the l2ptPartialFree should start empty
    ASSERT_NULL(pageallocator.pagePartialFree);

    struct Page * p1 = pagealloc_allocate_page();
    // After one allocation, it shouldn't be empty anymore
    ASSERT_NOT_NULL(pageallocator.pagePartialFree);
    // But it's length should be only one
    ASSERT_NULL(pageallocator.pagePartialFree->next);
    // The #allocated should have stayed the same as this node goes on l2ptPartialFree
    ASSERT_EQ(listlength(pageallocator.allocated), totalallocated);

    struct Page * pages[3];

    for (int i = 0; i < 2; i++) {
        pages[i] = pagealloc_allocate_page();
        ASSERT_NULL(pageallocator.pagePartialFree->next);
    }
    pages[2] = pagealloc_allocate_page();

    ASSERT_NULL(pageallocator.pagePartialFree);

    ASSERT_EQ(totalallocated + 1u, listlength(pageallocator.allocated));

    // Now as we allocate a 17th l1 pagetable, the #allocated should have grown by one
    struct Page * p17 = pagealloc_allocate_page();
    ASSERT_EQ(listlength(pageallocator.allocated), totalallocated + 1u);
    // but l2ptPartialFree should now be of length one again
    ASSERT_NOT_NULL(pageallocator.pagePartialFree);
    ASSERT_NULL(pageallocator.pagePartialFree->next);

    ASSERT_EQ(p1 + 1, pages[0]);

    // Assert pt2 comes after pt1 etc.
    for (int i = 0; i < 2; i++) {
        ASSERT_EQ(pages[i] + 1, pages[i+1]);
    }

    pagealloc_free_page(p17);

    for (int i = 0; i < 3; i++) {
        pagealloc_free_page(pages[i]);
    }

    pagealloc_free_page(p1);


    ASSERT_NOT_NULL(pageallocator.unused);
    ASSERT_EQ(listlength(pageallocator.unused),1278);
    ASSERT_EQ(listlength(pageallocator.allocated), 2);
    ASSERT_NULL(pageallocator.l2ptPartialFree);
    ASSERT_NULL(pageallocator.pagePartialFree);


    for (int i = 0; i < 100; i++) {
        struct Page * pages2[100];

        // Allocate 100 l2pts
        for (int i = 0; i < 100; i++) {
            pages2[i] = pagealloc_allocate_page();
        }

        // Now free them in a very different order
        for (int i = 0; i < 50; i++) {
            pagealloc_free_page(pages2[i]);
            pagealloc_free_page(pages2[99-i]);
        }

        ASSERT_NOT_NULL(pageallocator.unused);
        ASSERT_EQ(listlength(pageallocator.unused),1278);
        ASSERT_EQ(listlength(pageallocator.allocated), 2);
        ASSERT_NULL(pageallocator.l2ptPartialFree);
        ASSERT_NULL(pageallocator.pagePartialFree);
    }
})

TEST_CREATE(test_first_free, {
    ASSERT_EQ(first_free(0b000010), 0);
    ASSERT_EQ(first_free(0b000011), 2);
    ASSERT_EQ(first_free(0b000111), 3);
    ASSERT_EQ(first_free(0b111101), 1);
    ASSERT_EQ(first_free(0b111111), 6);
})
