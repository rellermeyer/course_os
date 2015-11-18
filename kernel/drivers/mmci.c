/**
 * @file MMCI (Multimedia Card Interface) Driver
 *
 * @author  Joseph Bourque, Matt Davidson, Weston Selleck \n
 *          QC and Support: Lane Kolbly
 *
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The driver for the SD card; provides an interface between the
 * operating system and the SD card controller. Currently set up
 * to use FIFO for input and output 
 *
 * TODO: Need to implement DMA support at some time in the future
 *       (Not needed until multi-threading implemented)
 *
 * Completed On: 4/20/2015           Last Updated: 5/5/2015
 */
#include "klibc.h"
#include "drivers/mmci.h"

// MMCI Definitions - Used to access SD card registers; DO NOT CHANGE!
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

// bit shift definitions - Used to modify SD card registers; DO NOT CHANGE!
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





/**
 * Initialize SD Card Method
 *
 * Initializes SD card putting it in the transfer state; this should
 * be called when the kernel boots up; readies the SD card for 
 * data transfer. Right now this function chooses the first available
 * SD card.
 *
 * TODO: Add to/split up this function in order to work with multiple
 *       SD cards i.e. let the user set up additional SD cards past
 *       the first
 *
 * @return 
 * 
 * If the SD card was initialized correctly; this returns 0; otherwise
 * this will return a negative integer value based on the value stored
 * in the status register
 */

int init_sd()
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
	
	return status();
}

/**
 * SD Capacity Method
 *
 * Returns the maximum capacity of the SD card in bytes; for our 
 * current needs it computes the size statically
 *
 * TODO: Eventually this needs to be set up to calculate the size
 *       of the SD card dynamically to account for different sized
 *       SD cards
 *
 * @return Returns the size of the SD card (in bytes)
 */
uint32_t sd_capacity()
{
	return 128*1024*1024;
}

/**
 * Transmit to SD Card Method
 *
 * Writes the contents of the FIFO buffer to the SD card; Writes
 * A single block at a time.
 *
 * @param  
 *
 * void* buffer - Holds a single block of data and treats it as a 
                  bitstream
 * @param  
 * 
 * uint32_t address - Holds the address of the block of the SD card 
 *                    to write to; the address is a multiple of the 
 *                    block size 
 * @return 
 * 
 * If the write was successful this returns 0; otherwise
 * this returns a negative integer based on the status
 * flags set             
 */
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
	return status();
}

/**
 * Receive from the SD Card Method
 *
 * Reads the contents of the FIFO buffer from the SD card; reads 
 * a single block at a time from the passed address and stores it
 * in the passed buffer
 *
 * @param  
 * 
 * void* buffer - Serves a a recepticle to place the information
 *                read from disk
 * @param 
 *
 * uint32_t address - stores the address on disk to read from
 *                    disk addresses are a mulitple of block 
 *                    size
 * 
 * @return 
 *
 * Returns 0 if the read was successful; otherwise returns a 
 * negative integer based on the value stored in the status
 * register
 */
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
	return status();
}

/**
 * Clear Method
 *
 * Resets all non-dynamic status flags 
 *
 * @return 
 *
 * Returns 0 if the clear was successful; otherwise returns a negative
 * integer based off the value stored in the status register
 */
int clear()
{
	// Return that the clear succeeded for now
	// Clear all status flags
	run_mmci(CLR, 0x2FF);
	return status() ; 
}


///// Helper Functions ///// Helper Functions ///// Helper Functions /////


/**
 * Push Bytes
 *
 * Helper method for the sd_transmit method; pushes bits into
 * the fifo buffer using the passed buffer space
 *
 * @param  
 *
 * void* buffer - An address which represents 1 block of data;
 *                provides a space for FIFO to interact with
 *                when writing to the SD card	   
 *
 * @return 
 *
 * Returns 0 if the write was successful; otherwise returns a 
 * negative integer based on the value stored in the status
 * register
 */
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
	return status();
}

/**
 * Pull Bytes Method
 *
 * Helper function for the sd_recieve method; pulls bits out of 
 * the FIFO buffer and stores them in the buffer passed into
 * the function (buffer represents 1 block of data)
 *
 * @param 
 * 
 * void* buffer - An address in memory representing 1 block
 *                of data; provides a space for FIFO to store
 *                the result of the read from the SD card
 *
 * @return 
 *
 * Returns 0 if the read was successful; otherwise returns a 
 * negative integer based on the value stored in the status
 * register
 */
int pull_bytes(void* buffer)
{
	int i = 0;
	while(READ_CNT){  
		((uint32_t*) buffer)[i] = read_mmci(FIFO);
		i++; 
	}
	
	// Return that the pull succeeded for now
	return status();
}

/**
 * Run MMCI Method
 *
 * This method is primarily used to run the various mmci commands
 * and it does so by using ARM assembly to store a value in register
 * corresponding to an MMCI command
 *
 * @param  
 *
 * uint32_t cmd - Stores the address offset based on the MMCI base
 *                address for a MMCI command.
 *
 * @param  
 *
 * uint32_t args - Represents the bits which will actually be set
 *                 on the register when running a specific command
 */
void run_mmci(uint32_t cmd, uint32_t args)
{
	uint32_t reg_address = MMCI_BASE+cmd;
	asm volatile("str %0,[%1]" : : "r" (args), "r" (reg_address));
}

/**
 * Read MMCI Method
 *
 * This method is primarily used for getting some response from a MMCI 
 * register (response register, status register, etc.)
 *
 * @param  
 *
 * uint32_t target - Passes in the target register to read; represented
 *                   as an offset to the MMCI base address

 * @return 
 *
 * Returns the value stored in the target register as an integer value
 */
uint32_t read_mmci(uint32_t target)
{
	uint32_t response;
	uint32_t reg_address = MMCI_BASE + target;
	asm volatile("ldr %0,[%1]" : "=r" (response) : "r" (reg_address));
	return response;
}

/**
 * Status Method 
 *
 * Helper method which handles error detection for the driver; because
 * multiple bits can be set due to multiple errors; combine bits where
 * neccessary to obtain error code
 *
 *@return
 *
 * if no error - will return 0
 * if -1 , error code 0
 * if -2 , error code 1
 * if -3 , error code 0 and 1
 */
int status(){
uint32_t stat = read_mmci(STATUS);
int error= stat & STATUS_ERR;
return (~error+1);
}
