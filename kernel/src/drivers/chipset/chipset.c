
#include <hardwareinfo.h>
#include <stdio.h>
#include <chipset.h>
#include <bcm2836.h>
#include <string.h>

ChipsetInterface chipset;

void init_chipset() {
    HardwareInfo * info = get_hardwareinfo();

    memset(&chipset, 0, sizeof(ChipsetInterface));
    
    switch (info->boardType) {
        case RaspBerryPiTwo:
            bcm2836_init();
            break;
        default: {
            kprintf("Board type not supported for interrupts \n");
            panic();
        }
    }
    
    for (size_t i = 0; i < sizeof(ChipsetInterface) / sizeof(uint32_t); i++) {
        if (((uint32_t **) &chipset)[i] == NULL) {
            kprintf("Chipset did not satisfy the required interface. Missing field %i\n", i);
            panic();
        }
    }
    
}
