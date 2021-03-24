#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H

#include <dtb.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum CpuType { ARM1176, CortexA7 } CpuType;

typedef enum BoardType { VersatilePB, RaspberryPiZero, RaspBerryPiTwo } BoardType;


// TODO: memory size?
typedef struct HardwareInfo {
    CpuType cpuType;
    BoardType boardType;
    size_t memory_size;
    size_t peripheral_base_address;
    size_t peripheral_region_size;
} HardwareInfo;

// Initialize the global hardware info struct. After this is ran, the get_hardwareinfo
// function can be used to retrieve it.
void init_hardwareinfo(struct DTHeader * dtb_h);

// Get a pointer to the hardwareinfo struct. This is a global struct containing information
// about the cpu type, and board/chipset type. May be expanded to contain more useful info.
HardwareInfo * get_hardwareinfo();
void print_hardwareinfo();
BoardType detect_boardtype(struct DTHeader * dtb_h);

// Checks whether given address is in area occupied by MMIO
bool address_in_reserved_region(size_t address);

#endif
