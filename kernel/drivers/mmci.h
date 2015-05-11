/*****************************************************************
 * Crude SD Card (MMCI) Driver                                   *
 * Programmed By: Joseph Bourque, Matt Davidson, Weston Selleck  *
 * Framework, QC, and Support: Lane Kolbly                       *
 * Completed: 4/20/2015    Last Updated: 4/20/2015               *
 *****************************************************************/
#ifndef HEADER_FILE
#define HEADER_FILE

// External definitions
#define SD_BLOCK_SIZE 512

// Function Prototypes
void run_mmci(uint32_t cmd, uint32_t args);
uint32_t read_mmci(uint32_t target);
int status();
int push_bytes(void* buffer);
int pull_bytes(void* buffer);
int init_sd();
uint32_t sd_capacity();
int sd_transmit(void* buffer, uint32_t address);
int sd_receive(void* buffer, uint32_t address);
int clear();
int status();

#endif
