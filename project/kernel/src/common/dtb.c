#include <dtb.h>
#include <string.h>
#include <klibc.h>
#include <vm2.h>

extern size_t __DTB_START[];


// DTB is in big endian, so it must be converted to little endian
static inline uint32_t fix_endian(const uint32_t num) {
    char * ptr = (char *)&num;
    int ret;

    ret = ptr[0] << 24;
    ret |= ptr[1] << 16;
    ret |= ptr[2] << 8;
    ret |= ptr[3];

    return ret;
}

// Advances past the begin node and returns name of the node
char * parse_begin_node(void ** curr_address) {
    *curr_address += sizeof(uint32_t);
    char * string_address = (char *)*curr_address;
    *curr_address += strlen(string_address) + 1;
    *curr_address = (void*)ALIGN(*curr_address,4);
    return string_address;
}

// Counts the number of characters until the next slash
uint32_t get_chars_to_slash(char * string) {
    for (uint32_t i = 0;; i++) {
        if (string[i] == '/' || string[i] == '\0') return i;
    }
    return 0;
}


// Performs an allocation-free traversal to find the requested property in the DTB
struct DTProp * dtb_get_property(struct DTHeader * dtb_h, char * path, char * property) {
    // Probably shouldn't use FATAL here.
    if (fix_endian(dtb_h->magic) != 0xd00dfeed) {
        /* FATAL("Wrong dtb header %x", dtb_h->magic); */
        simple_panic();

    }

    uint32_t current_level = 0;
    uint32_t next_level = 1;

    void * curr_address = (void *)dtb_h + fix_endian(dtb_h->off_dt_struct);

    while (curr_address <
           (void *)dtb_h + fix_endian(dtb_h->off_dt_struct) + fix_endian(dtb_h->size_dt_struct)) {
        switch (fix_endian(*(uint32_t *)curr_address)) {
            case FDT_BEGIN_NODE: {
                char * node_name = parse_begin_node(&curr_address);

                if (strlen(path) == 0) {  // Properties precede nodes. If we find a property in the
                                          // path where it should be, we are lost.
                    return NULL;
                }

                current_level++;
                if (current_level == next_level) {  // Avoid looking at nodes which don't match the
                                                    // nesting level we're looking for in the path
                    uint32_t chars_to_slash = get_chars_to_slash(path);
                    uint32_t node_name_size = strlen(node_name);
                    if (strlen(node_name) == 0 ||
                        strncmp(path,
                                node_name,
                                chars_to_slash < node_name_size ? chars_to_slash
                                                                : node_name_size) == 0) {
                        next_level++;
                        path += chars_to_slash;  // Advance path past the slash
                        if (path[0] == '/') path++;
                    }
                }

                break;
            }
            case FDT_END_NODE: {
                curr_address += sizeof(uint32_t);
                current_level--;
                if (strlen(path) == 0) {
                    return NULL;
                }  // We have exited the path we were looking for
                break;
            }
            case FDT_PROP:
                curr_address += sizeof(uint32_t);
                struct DTProp * prop = (struct DTProp *)curr_address;
                if (strlen(path) == 0) {
                    // We are in the node that we looking for. Time to check if this is the property
                    // we were looking for
                    char * prop_name = (char *)((void *)dtb_h + fix_endian(dtb_h->off_dt_strings) +
                                                fix_endian(prop->nameoff));
                    if (strcmp(prop_name, property) == 0) {
                        // Found it!
                        return prop;
                    }
                }
                // Skip over the property data
                curr_address += sizeof(struct DTProp) + fix_endian(prop->len);
                curr_address = (void*)ALIGN(curr_address,4);
                break;
            case FDT_NOP:
                curr_address += sizeof(uint32_t);
                break;
            case FDT_END:
                return NULL;
                break;
        }
    }

    return NULL;
}

// Wraps a property containing 2 uint32's, also fixing the endianness of the elements.
struct DTProp2UInt dtb_wrap_2uint_prop(struct DTHeader * dtb_h, struct DTProp * prop) {
    assert(fix_endian(prop->len) == 8);
    struct DTProp2UInt wrapped = {
        (char *)((void *)dtb_h + fix_endian(dtb_h->off_dt_strings) + fix_endian(prop->nameoff)),
        fix_endian(*((uint32_t *)prop + 2)),
        fix_endian(*((uint32_t *)prop + 3))};
    return wrapped;
}

struct DTPropString dtb_wrap_string_prop(struct DTHeader * dtb_h, struct DTProp * prop) {
    struct DTPropString wrapped = {
        (char *)((void *)dtb_h + fix_endian(dtb_h->off_dt_strings) + fix_endian(prop->nameoff)),
        fix_endian(prop->len),
        (char *)(prop + 1)};
    return wrapped;
}
