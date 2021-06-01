// https://devicetree-specification.readthedocs.io/en/v0.1/flattened-format.html

#ifndef DTB_H
#define DTB_H

#include <stdint.h>
#include <klibc.h>
#include <string.h>

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

struct DTProp {
  uint32_t len;
  uint32_t nameoff;
};

struct DTProp2UInt {
  char* name;
  uint32_t first;
  uint32_t second;
};

struct DTPropString {
  char* name;
  uint32_t len;
  char* val;
};

enum StructureToken {
    FDT_BEGIN_NODE = 0x1,
    FDT_END_NODE = 0x2,
    FDT_PROP = 0x3,
    FDT_NOP = 0x4,
    FDT_END = 0x9,
};

struct DTProp* dtb_get_property(struct DTHeader * dtb_h, char * path, char * property);

struct DTProp2UInt dtb_wrap_2uint_prop(struct DTHeader* dtb_h,struct DTProp* prop);

struct DTPropString dtb_wrap_string_prop(struct DTHeader* dtb_h,struct DTProp* prop);


#endif
