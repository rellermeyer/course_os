#ifndef HEADER_FILE
#define HEADER_FILE

// External definitions
#define SD_BLOCK_SIZE 512

// Function Prototypes
void run_mmci(uint32_t cmd, uint32_t args);
uint32_t read_mmci(uint32_t target);
int push_bytes(uint32_t* buffer);
int pull_bytes(uint32_t* buffer);
void init_sd();
uint32_t sd_capacity();
int sd_transmit(uint32_t* buffer, uint32_t address);
int sd_receive(uint32_t* buffer, uint32_t address);
int clear();

#endif
