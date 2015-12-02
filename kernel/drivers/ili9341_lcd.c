#include "drivers/ili9341_registers.h"
#include "drivers/mmci.h"


#define TFTWIDTH   240
#define TFTHEIGHT  320

int lcd_init();
void setAddrWindow(int x1, int y1, int x2, int y2);

// TODO: Need Delay function
int lcd_init() {

    run_mmci(ILI9341_SOFTRESET, 0);
//    delay(50);
    run_mmci(ILI9341_DISPLAYOFF, 0);

    run_mmci(ILI9341_POWERCONTROL1, 0x23);
    run_mmci(ILI9341_POWERCONTROL2, 0x10);
    run_mmci(ILI9341_VCOMCONTROL1, 0x2B2B);
    run_mmci(ILI9341_VCOMCONTROL2, 0xC0);
    run_mmci(ILI9341_MEMCONTROL, ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
    run_mmci(ILI9341_PIXELFORMAT, 0x55);
    run_mmci(ILI9341_FRAMECONTROL, 0x001B);

    run_mmci(ILI9341_ENTRYMODE, 0x07);

    run_mmci(ILI9341_SLEEPOUT, 0);
//    delay(150);
    run_mmci(ILI9341_DISPLAYON, 0);
//    delay(500);
    setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
    return 0;
}

void setAddrWindow(int x1, int y1, int x2, int y2) {

    uint32_t t;

    t = x1;
    t <<= 16;
    t |= x2;
    run_mmci(ILI9341_COLADDRSET, t);
    t = y1;
    t <<= 16;
    t |= y2;
    run_mmci(ILI9341_PAGEADDRSET, t);
}
