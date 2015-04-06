typedef struct opentable_entry 
{
	struct opentable_entry*;
	int; //file descriptor
	int; //keeps track of where we are reading
	struct opentable_entry*;	
} opentable_entry;

opentable_entry* add_to_opentable(int);
int delete_from_opentable(opentable_entry*);

