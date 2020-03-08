#include <tlb_cache_id_allocator.h>
#include <test.h>

TEST_CREATE(tlb_simple, {
    struct TLBDescriptor desc = request_tlb_descriptor();

    uint32_t id = desc.tlb_cache_id;
    uint32_t iteration = desc.cache_iteration;

    if(check_and_update(&desc)) {
        ASSERT_NEQ(iteration, desc.cache_iteration);
    } else {
        ASSERT_EQ(iteration, desc.cache_iteration);
    }

    ASSERT_EQ(id, desc.tlb_cache_id);
})

TEST_CREATE(tlb_intensive, {
    // This tests asserts the properties of the TLBDescriptor in an extensive way,
    // to ensure at least 1 overflow takes place
    for(size_t i = 0; i < 666; i++) {
        struct TLBDescriptor desc = request_tlb_descriptor();

        uint32_t id = desc.tlb_cache_id;
        uint32_t iteration = desc.cache_iteration;

        if(check_and_update(&desc)) {
            ASSERT_NEQ(iteration, desc.cache_iteration);
        } else {
            ASSERT_EQ(iteration, desc.cache_iteration);
        }

        ASSERT_EQ(id, desc.tlb_cache_id);
    }

    // Everything should have definitely been allocated by now.
    ASSERT(tlb_everything_allocated);
})
