#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H

typedef enum CpuType { ARM1176, CortexA7 } CpuType;

typedef enum BoardType { VersatilePB, RaspberryPiZero, RaspBerryPiTwo } BoardType;


// TODO: memory size?
typedef struct HardwareInfo {
    CpuType cpuType;
    BoardType boardType;
} HardwareInfo;

// Initialize the global hardware info struct. After this is ran, the get_hardwareinfo
// function can be used to retrieve it.
void init_hardwareinfo();

// Get a pointer to the hardwareinfo struct. This is a global struct containing information
// about the cpu type, and board/chipset type. May be expanded to contain more useful info.
HardwareInfo * get_hardwareinfo();
void print_hardwareinfo();
BoardType detect_boardtype();

#endif
