/*****************************************************************
 * Crude SD Card (MMCI) Driver                                   *
 * Programmed By: Joseph Bourque, Matt Davidson, Weston Selleck  *
 * Framework, QC, and Support: Lane Kolbly                       *
 * Completed: 4/20/2015    Last Updated: 4/20/2015               *
 *****************************************************************/
#include "klibc.h"
#include "mmci.h"

// MMCI Definitions
#define MMCI_BASE 0x10005000
#define CMD 0x00C
#define DCTRL 0x02C
#define FIFO 0x080
#define FIFO_CNT 0x048
#define STATUS 0x34
#define ARG 0x008
#define CLR 0x038
#define RESP_0 0x14
#define RESP_1 0x18
#define RESP_2 0x1C
#define RESP_3 0x20
#define DATA_LEN 0x028

// bit shift definitions
#define EXE (1 << 10)
#define RESP (1 << 6)
#define LRESP (1 << 7)
#define RCA_MASK 0xFFFF0000
#define SET_WRITE 1 | (9 << 4)
#define SET_READ 3 | (9 << 4)
#define DISABLE 0x0 | (9 << 4)
#define READ_CNT (read_mmci(STATUS) & (1 << 21))
#define WRITE_CNT (read_mmci(FIFO_CNT) > 0)
#define STATUS_ERR 0x3F






// Initializes SD card putting it in the transfer state; this should
// be called when the kernel boots up; readies the SD card for 
// data transfer
void init_sd()
{
	asm volatile("cpsie if");
	int rca = 0;

	// let the SD card know you're about to call one of the ACMD commands
	run_mmci(CMD, 55 | EXE);

	// Set up the argument register for the next command
	run_mmci(ARG, 123);

	// Set & Execute the ACMD set voltage command
	run_mmci(CMD, 41 | EXE);

	// Set & Execute the get card identifier command
	run_mmci(CMD, 2 | EXE | RESP | LRESP);

	// Set & execute the obtain RCA command; this will allow you
	// to identifiy and select a particular SD card to read/write
	// to in the event you have more than one card
	run_mmci(CMD, 3 | EXE | RESP);

        // obtain RCA address and mask out the status bits
        rca = (read_mmci(RESP_0) & RCA_MASK);
        
        // Load masked RCA address into the argument register
        run_mmci(ARG, rca);

	// Run the select card command and point it at the card loaded
        // in the argument register
	run_mmci(CMD, 7 | EXE | RESP);

	// Set the data length register to 512 to indicate the amount
	// amount of data to be moved is 512 bytes
	// Note: This sets the value of FIFO count as well
	run_mmci(DATA_LEN, SD_BLOCK_SIZE);

	// Eventually print some status message here if the SD card
	// failed to initialize properly but for now just print 
	// that it was loaded OK
	os_printf("\nSD card ready for transfer\n");
}

// Returns the maximum capacity of the SD card; Since our SD card is
// set at 512kB only this is just a stub function for now but this
// may need to be expanded later in the event we want to support
// variable SD card size i.e. find the capacity of the SD card vs.
// using a static capacity
uint32_t sd_capacity()
{
	return SD_BLOCK_SIZE*1024;
}

// Writes the contents of the FIFO buffer to the SD
// card; writes a single block; returns 0 if the
// transmission was successful and 1 if there was 
// an error
int sd_transmit(void* buffer, uint32_t address)
{
	//Set the block length on the SD card
	run_mmci(ARG, SD_BLOCK_SIZE);
	run_mmci(CMD, 16 | EXE | RESP);

	//Set up the write command
	run_mmci(ARG, address);
	run_mmci(CMD, 24 | EXE | RESP);
	push_bytes(buffer);

	// Return that write succeeded for now
	return 0;
}

// Reads the contents of the FIFO buffer to the SD
// card; reads a single block and returns 0 if the
// transmission succeeded and 1 if there was an error
int sd_receive(void* buffer, uint32_t address)
{
	//Clear out FIFO and set to read
	run_mmci(DCTRL, DISABLE);
	run_mmci(DCTRL, SET_READ);
	//Set address to read from & send the command to read
	run_mmci(ARG, address);
	run_mmci(CMD, 17 | EXE | RESP);
	
	pull_bytes(buffer);

	// Return that read succeeded for now
	return 0;
}

// Resets all non-dynamic status flags; returns 0
// if the clear was successful and 1 if there was
// an error
int clear()
{
	// Return that the clear succeeded for now
	// Clear all status flags
	run_mmci(CLR, 0x2FF);
	return 0; 
}



///// Helper Functions ///// Helper Functions ///// Helper Functions /////



// Push bits into FIFO buffer from the address passed 
// into the push bits function; Returns 0 if successful 
// and 1 if an error occurred
int push_bytes(void* buffer)
{
	//Clear out FIFO and set to write
	run_mmci(DCTRL, DISABLE);
	run_mmci(DCTRL, SET_WRITE);
	int i = 0;
	while(WRITE_CNT){
		run_mmci(FIFO, ((uint32_t*)buffer)[i]);
		i++;
	}
	
	// Return that the push succeeded for now
	return 0;
}

// Pull bits out of the FIFO buffer and store them at the 
// address passed into the pull bits function; returns 0 
// if successful and 1 if an error occurred
int pull_bytes(void* buffer)
{
	int i = 0;
	while(READ_CNT){  
		((uint32_t*) buffer)[i] = read_mmci(FIFO);
		i++; 
	}
	
	// Return that the pull succeeded for now
	return 0;
}

// Stub function used to run MMCI commands
void run_mmci(uint32_t cmd, uint32_t args)
{
	uint32_t reg_address = MMCI_BASE+cmd;
	asm volatile("str %0,[%1]" : : "r" (args), "r" (reg_address));
}

uint32_t read_mmci(uint32_t target)
{
	uint32_t response;
	uint32_t reg_address = MMCI_BASE + target;
	asm volatile("ldr %0,[%1]" : "=r" (response) : "r" (reg_address));
	return response;
}

uint32_t status(){

uint32_t status = read_mmci(STATUS);

if ((status && STATUS_ERR)!=0){
	return 1;
}
//no error
else return 0;
}