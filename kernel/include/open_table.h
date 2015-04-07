typedef struct file_descriptor 
{
	file* file;
	struct file_descriptor* previous;
	int offset; //keeps track of where we are reading
	struct file_descriptor* next;
	int open; // in kopen() and/or kcreate set this to 10; in kclose() set this to 0	
} file_descriptor;

file_descriptor* add_to_opentable(file_descriptor*);
int delete_from_opentable(file_descriptor*);

