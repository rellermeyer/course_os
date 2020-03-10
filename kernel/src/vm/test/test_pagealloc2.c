#include <test.h>
#include <pmm.h>
#include <string.h>
#include <interrupt.h>

// Warning: This method is O(n) and is thus quite slow on long lists.
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

TEST_CREATE(test_pmm_constants, {
    ASSERT_EQ(sizeof(struct MemorySliceInfo), 16);
    ASSERT_EQ(sizeof(union MemorySlice) / sizeof(struct MemorySliceInfo), SLICEINFO_PER_SLICE);
    ASSERT_EQ(sizeof(union MemorySlice) / sizeof(struct L1PageTable), 1);
    ASSERT_EQ(sizeof(union MemorySlice) / sizeof(struct L2PageTable), L2TABLES_PER_SLICE);
    ASSERT_EQ(sizeof(union MemorySlice) / sizeof(struct Page), PAGES_PER_SLICE);
    ASSERT_EQ(sizeof(struct Page), PAGE_SIZE);
})

TEST_CREATE(test_allocate_pt, {
    size_t unused = listlength(physicalMemoryManager.unused);
    size_t allocated = listlength(physicalMemoryManager.allocated);

    struct L1PageTable * pt = pmm_allocate_l1_pagetable();

    // Just try to overwrite everything a few times so if something breaks we'll hopefully see in the next tests.
    memset(pt, 0, 1024 * 16);
    memset(pt, 1, 1024 * 16);

    ASSERT_NOT_NULL(physicalMemoryManager.unused);
    ASSERT_EQ(listlength(physicalMemoryManager.unused), unused - 1);
    ASSERT_NOT_NULL(physicalMemoryManager.allocated);
    ASSERT_EQ(listlength(physicalMemoryManager.allocated), allocated + 1);

    pmm_free_l1_pagetable(pt);

    ASSERT_EQ(listlength(physicalMemoryManager.unused), unused);
    ASSERT_EQ(listlength(physicalMemoryManager.allocated), allocated);
})

TEST_CREATE(test_allocate_many_pt, {
    size_t total = listlength(physicalMemoryManager.unused);
    size_t totalallocated = listlength(physicalMemoryManager.allocated);

    const size_t amount = 1026;

    struct L1PageTable * pages[amount];

    for (int i = 0; i < amount; i++) {
        pages[i] = pmm_allocate_l1_pagetable();
        ASSERT_NOT_NULL(pages[i]);
        ASSERT_EQ(listlength(physicalMemoryManager.unused), total - (i + 1u));
        ASSERT_EQ(listlength(physicalMemoryManager.allocated), totalallocated + i + 1);
    }

    struct L1PageTable * pt = pmm_allocate_l1_pagetable();

    ASSERT_NOT_NULL(physicalMemoryManager.unused);
    ASSERT_EQ(listlength(physicalMemoryManager.unused), total - (amount + 1));
    ASSERT_NOT_NULL(physicalMemoryManager.allocated);

    ASSERT_EQ(listlength(physicalMemoryManager.allocated), totalallocated + (amount + 1));

    pmm_free_l1_pagetable(pt);


    for (int i = 0; i < amount; i++) {
        pmm_free_l1_pagetable(pages[i]);
    }


    ASSERT_NOT_NULL(physicalMemoryManager.unused);
    ASSERT_EQ(listlength(physicalMemoryManager.unused), total);
    ASSERT_EQ(listlength(physicalMemoryManager.allocated), totalallocated);
})

// Test the magic :sparkles:
TEST_CREATE(test_get_sliceinfo, {
    struct L1PageTable * pt = pmm_allocate_l1_pagetable();

    struct MemorySliceInfo * info = pmm_get_sliceinfo_for_slice((union MemorySlice *) pt);

    ASSERT_EQ(&info->slice->l1pt, pt);

    pmm_free_l1_pagetable(pt);
})

TEST_CREATE(test_doubly_linked_sliceinfo, {
    struct MemorySliceInfo * curr = physicalMemoryManager.unused;
    uint32_t length = listlength(curr);

    uint32_t i = 0;
    do {
        i++;
        ASSERT_EQ(curr->next->prev, curr);
        curr = curr->next;
    }   while(curr->next != NULL);

    ASSERT_EQ(i, length-1);
})

//TODO: test_allocate_page
//TODO: test_allocate_l2pt

size_t first_free(uint16_t filled);

TEST_CREATE(test_first_free, {
    ASSERT_EQ(first_free(0b000010), 0);
    ASSERT_EQ(first_free(0b000011), 2);
    ASSERT_EQ(first_free(0b000111), 3);
    ASSERT_EQ(first_free(0b111101), 1);
    ASSERT_EQ(first_free(0b111111), 6);
})
