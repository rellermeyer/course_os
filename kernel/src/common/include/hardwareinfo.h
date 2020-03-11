#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H

typedef enum CpuType {
    ARM1176,
    CortexA7
} CpuType;

typedef enum BoardType {
    VersatilePB,
    RaspberryPiZero,
    RaspBerryPiTwo
} BoardType;


// TODO: memory size?
typedef struct HardwareInfo {
    CpuType cpuType;
    BoardType boardType;
} HardwareInfo;

void init_hardwareinfo();

// Get a pointer to the hardwareinfo struct.
HardwareInfo * get_hardwareinfo();
void print_hardwareinfo();
BoardType detect_boardtype();

#endif
