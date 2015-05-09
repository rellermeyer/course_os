#include <stdint.h>

/* 
	Contained within this file is the base addresses to many of the Rasberry Pi's peripherals, 
	along with definitions for some of the important offsets from these case addresses.
*/
#define USB_BASE_ADDRESS 0x20980000

#define USB_MDIO_CNTL (USB_BASE_ADDRESS +0x080) // MDIO interface control
#define USB_MDIO_GEN (USB_BASE_ADDRESS + 0x084) // Datat for MDIO interface
#define USB_VBUS_DRV (USB_BASE_ADDRESS + 0x088) // Vbus and other miscellaneous controls

// BSC stands for Broadcom Serial Controller, from the documentation, it looks like its used for HDMI
#define BSC_MASTER_0 0x20205000
#define BSC_MASTER_1 0x20804000
#define BSC_MASTER_2 0x20805000
//Offsets can be used with any of the above three BSC register BSC register bases
#define BSC_CONTROL	 0x0
#define BSC_STATUS 0x4
#define BSC_DATA_LENGTH 0x8
#define BSC_SLAVE_ADDRESS 0xC
#define BSC_FIFO 0x10
#define BSC_CLOCK_DIVIDER 0x14
#define BSC_DATA_DELAY 0x18

