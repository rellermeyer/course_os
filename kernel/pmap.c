#include "include/mmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"
#include "include/pmap.h"

unsigned int next_frame = PMSTART;

unsigned int get_frame(unsigned int num_frames){
	unsigned int frame = next_frame;

	if(next_frame + (num_frames*FRAMESIZE) < (unsigned int)PMEND)
		next_frame = next_frame + (num_frames * FRAMESIZE);
	else
		next_frame = 0;

	return next_frame;
}

