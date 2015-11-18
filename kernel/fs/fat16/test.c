#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
	char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "Current working dir: %s\n", cwd);
   else
       perror("getcwd() error");	

	char* buf = (void*) malloc(sizeof(char) * 512);
	char* original_buf = buf;
	*buf = "courseOSfsaaaaaaaaaaaaaaaaaaaaa";
	buf +=32;
	
	*((int*)buf) = (int)1;
	buf +=4;
	
	buf = (int*)buf;

	*(buf) = (int)2;
	buf +=4;
	
	*(buf) = (int)512;
	buf +=4;

	*(buf) = (int)2;
	buf +=4;
	
	*(buf) = (int)100;
	buf +=4;

	*(buf) = (int)900;
	buf +=4;

	*(buf) = (int)11;
	buf +=4;

	*(buf) = (int)12;
	buf +=4;

	*(buf) = (int)333;
	buf +=4;

	*(buf) = (int)444;
	buf +=4;

	// 12 512000 512 2 100 900 11 12 333 444
	fprintf("TESTING***************\n")
	int fd = open("superblock_1.data", 'O_RDWR');
	write(fd, original_buf, 512);
	close(fd);	


	// FILE *params_file = fopen("superblock_1.data", "w"); // write only
	// 	// test to ensure that the file was actually created and exists: 
	// 	if (params_file == NULL){
	// 		printf("1. Error! Could not create file\n"); 
	// 		exit(-1); // must include stdlib.h 
	// 	}//end if

	// 	fprintf(params_file, "PARAMS\n");
	// 	fprintf(params_file, "nx=%d,ny=%d,nz=%d\n", nx, ny, nz);
	// 	fprintf(params_file, "dx=%lg,dy=%lg,dz=%lg\n", dx, dy, dz);
	// 	fprintf(params_file, "numFiles=%d\n", numFiles);
 //        fprintf(params_file, "numParticles=%d\n", numParticles);
	// 	fclose(params_file);



	return 0;
}
