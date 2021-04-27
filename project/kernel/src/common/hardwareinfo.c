#include <bcm2835.h>
#include <bcm2836.h>
#include <constants.h>
#include <debug.h>
#include <dtb.h>
#include <hardwareinfo.h>
#include <interrupt.h>
#include <stdio.h>
#include <vm2.h>

static HardwareInfo hardware_info;

extern size_t __DTB_START[];

BoardType detect_boardtype(struct DTHeader * dtb_h) {
    struct DTProp * prop = dtb_get_property(dtb_h, "/", "model");
    assert(prop != NULL);
    struct DTPropString prop_str = dtb_wrap_string_prop(dtb_h, prop);

    if (strncmp(prop_str.val, "Raspberry Pi 2", strlen("Raspberry Pi 2")) == 0) {
        return RaspBerryPiTwo;
    } else if (strncmp(prop_str.val, "Raspberry Pi Zero", strlen("Raspberry Pi Zero")) == 0) {
        return RaspberryPiZero;
    } else {
        FATAL("Unknown boardtype");
    }
}

size_t detect_memory_size(struct DTHeader * dtb_h) {
    struct DTProp * mem_prop = dtb_get_property(dtb_h, "/memory", "reg");
    assert(mem_prop != NULL);  // Failed to find memory node in the DTB

    struct DTProp2UInt memory_prop = dtb_wrap_2uint_prop(
        dtb_h,
        mem_prop);  // First element is address and second element is size of available memory
    assert(memory_prop.first == 0);  // There can be multiple memory declarations in the DTB, but
                                     // we're assuming there's only 1, and it starts at address 0

    if (memory_prop.second == 0) {
        memory_prop.second = Gibibyte;  // TODO: Dump the Rpi 2 DTB so this is not needed
    }
    return memory_prop.second;
}

CpuType detect_cputype() {
    uint32_t reg;

    // read system register
    asm("mrc p15,0,%0,c0,c0,0" : "=r"(reg));

    uint32_t type = (reg >> 4u) & 0xFFFu;

    switch (type) {
        case 0xB76:
            return ARM1176;  // bcm2835
        case 0xC07:
            return CortexA7;  // bcm2836
        default:
            FATAL("Unknown cputype");
    }
}

// TODO: detect hardware info.
// Such as: CPU and RAM size.

void init_hardwareinfo(struct DTHeader * dtb_h) {
    if (dtb_h == NULL) {
        dtb_h = (struct DTHeader *)
            __DTB_START;  // DTB not passed by bootloader, we are in QEMU. Use Embedded DTB.
    }

    BoardType boardType = detect_boardtype(dtb_h);
    size_t memory_size = detect_memory_size(dtb_h);
    size_t peripheral_base_address;
    size_t peripheral_region_size;
    switch (boardType) {
        case RaspBerryPiTwo:
            peripheral_base_address = BCM2836_PERIPHERALS_PHYSICAL_BASE;
            peripheral_region_size = 21 * Mebibyte;
            break;
        case RaspberryPiZero:
            peripheral_base_address = BCM2835_PERIPHERALS_PHYSICAL_BASE;
            peripheral_region_size = 18 * Mebibyte;
            break;
        default:
            FATAL("Peripheral address for board type not implemented");
    }
    hardware_info = (HardwareInfo){
        .cpuType = detect_cputype(),
        .boardType = boardType,
        .memory_size = memory_size,
        .peripheral_base_address = peripheral_base_address,
        .peripheral_region_size = peripheral_region_size,
    };
}

HardwareInfo * get_hardwareinfo() {
    return &hardware_info;
}

void print_hardwareinfo() {
    kprintf("==============================================\n");
    kprintf("Hardware info detected:\n");
    switch (hardware_info.cpuType) {
        case ARM1176:
            kprintf("CPU type: %s\n", "ARM 1176");
            break;
        case CortexA7:
            kprintf("CPU type: %s\n", "Cortex A7");
            break;
    }

    switch (hardware_info.boardType) {
        case RaspberryPiZero:
            kprintf("Board type: %s\n", "Raspberry Pi zero or 1b+");
            break;
        case RaspBerryPiTwo:
            kprintf("Board type: %s\n", "Raspberry Pi two");
            break;
        case VersatilePB:
            kprintf("Board type: %s\n", "Versatile PB");
            break;
    }

    kprintf("==============================================\n");
}

bool address_in_reserved_region(size_t address) {
    return ((address - KERNEL_VIRTUAL_OFFSET) >= get_hardwareinfo()->peripheral_base_address &&
            (address - KERNEL_VIRTUAL_OFFSET) < get_hardwareinfo()->peripheral_base_address +
                                                    get_hardwareinfo()->peripheral_region_size);
}
