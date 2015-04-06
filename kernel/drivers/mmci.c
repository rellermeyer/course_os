#include <stdlib.com>

/** Stub Driver; will change into real driver as we progress **/

// Fake hard drive; will replace once we figure out how to actually
// write to the SD card controller
int* sd_card[32768] = {0};

// Fake base address; this will be redefined when we find out what 
// this is

#define MMCI_DATA_LEN MMCI_BASE+0x28
#define MMCI_DATA_CTRL MMCI_BASE+0x02C
#define MMCI_CLEAR MMCI_BASE+0x038
#define MMCI_DATA_TIMER MMCI_BASE+0x24
#define MMCI_DATA_CNT MMCI_BASE+0x030
#define MMCI_BASE 0x10005000
#define BLOCK_SIZE 2048
void initialize()
{
	int data_ctrl;
	asm volatile("mov %0, MMCI_DATA_CTRL" : "=r" (data_ctrl));
	data_ctrl = (data_ctrl & 0xFFFFFF00) | 0x30;
	asm volatile("mov %0, (data_ctrl)" : "=r" MMCI_DATA_CTRL);
}

// Move from controller to card (write)
void transmit()
{
        /** Sometime in the future, we need to specify the starting
            block to write right here...m'mkay? (more specifically
            we need to find out how to specify which blocks to 
            read/write on the SD card and how to map those to
            memory) **/
	int data_ctrl;
	int data_len;

        // Reset data length to 1 block
	asm volatile("mov %0, MMCI_DATA_LEN" : "=r" (data_len));
        data_len = (data_len & 0xFFFF0000) | 0x800;
        asm volatile("mov %0, (data_len)" : "=r" MMCI_DATA_LEN);
        
        // Flip direction to 0 and enable to 1 to start writing
	asm volatile("mov %0, MMCI_DATA_CTRL" : "=r" (data_ctrl));
	data_ctrl = (data_ctrl & 0xFFFFFFFD) | 0x1;
	asm volatile("mov %0, (data_ctrl)" : "=r" MMCI_DATA_CTRL);
}

void recieve()
{
        /** Sometime in the future, we need to specify the starting
            block to write right here...m'mkay? (more specifically
            we need to find out how to specify which blocks to
            read/write on the SD card and how to map those to
            memory) **/
        int data_ctrl;
        int data_len;

        // Reset data length to 1 block
        asm volatile("mov %0, MMCI_DATA_LEN" : "=r" (data_len));
        data_len = (data_len & 0xFFFF0000) | 0x800;
        asm volatile("mov %0, (data_len)" : "=r" MMCI_DATA_LEN);

        // Flip direction to 1 and enable to 1 to start writing
        asm volatile("mov %0, MMCI_DATA_CTRL" : "=r" (data_ctrl));
        data_ctrl = data_ctrl | 0x3;
        asm volatile("mov %0, (data_ctrl)" : "=r" MMCI_DATA_CTRL);
}

