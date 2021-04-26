// We have to define these functions in a header file, for C to be able to access them
// If you have a better way of doing this, be my guest

void handle_swi(void);
void handle_irq(void);

void* get_previous_sp(void);
void save_to_previous_sp(void);

