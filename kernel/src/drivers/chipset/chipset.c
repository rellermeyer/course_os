
#include <bcm2836.h>
#include <chipset.h>
#include <hardwareinfo.h>
#include <stdio.h>
#include <string.h>

// We use putc (and printf) before the chipset is initialized already.
// This stub makes sure that nothing crashes if you do that.
void uart_putc_stub(char c __attribute__((unused)), int channel __attribute__((unused))) {}

ChipsetInterface chipset = {
    .uart_putc = uart_putc_stub  // Stub putc
};

void init_chipset() {
    HardwareInfo* info = get_hardwareinfo();

    memset(&chipset, 0, sizeof(ChipsetInterface));

    switch (info->boardType) {
        case RaspBerryPiTwo:
            bcm2836_init();
            break;
        default: {
            FATAL("Board type not supported for interrupts \n");
        }
    }

    for (size_t i = 0; i < sizeof(ChipsetInterface) / sizeof(uint32_t); i++) {
        if (((uint32_t **) &chipset)[i] == NULL) {
            FATAL("Chipset did not satisfy the required interface. Missing field %i\n", i);
        }
    }
}
