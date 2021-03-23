// We have to define these functions in a header file, for C to be able to access them
// If you have a better way of doing this, be my guest

void _handle_swi(void);

void save_state_irq(void);
void load_state_irq(void);

void enter_priviledged_previous_mode(void);

void* get_previous_sp(void);
void save_to_previous_sp(void);

