#include <hardwareinfo.h>
#include <interrupt.h>
#include <stdio.h>

static HardwareInfo hardware_info;

BoardType detect_boardtype() {
    uint32_t reg;

    // read system register
    asm("mrc p15,0,%0,c0,c0,0" : "=r"(reg));

    uint32_t type = (reg >> 4u) & 0xFFFu;

    switch (type) {
        case 0xB76:
            return RaspberryPiZero;  // bcm2835
        case 0xC07:
            return RaspBerryPiTwo;  // bcm2836
        default:
            FATAL("Unknown boardtype");
    }
}

// TODO: detect hardware info.
// Such as: CPU and RAM size.
void init_hardwareinfo() {
    hardware_info = (HardwareInfo){
        .cpuType = ARM1176,
        .boardType = detect_boardtype(),
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
