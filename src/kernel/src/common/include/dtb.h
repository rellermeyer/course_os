#ifndef DTB_H
#define PMM_H

#include <stdint.h>
#include <klibc.h>

struct DTHeader {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

enum StructureToken {
    FDT_BEGIN_NODE = 0x1,
    FDT_END_NODE = 0x2,
    FDT_PROP = 0x3,
    FDT_NOP = 0x4,
    FDT_END = 0x9,
};

void dtb_get_property(struct DTHeader* dtbh, char* path, char* property);


#endif
