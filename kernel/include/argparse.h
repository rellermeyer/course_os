#include <stdint.h>

void run_argparse(uint32_t p_bootargs);
void parse_arguments(int argc, char **argv);
int analyze_args(char **argv);
char* read_cmdline_tag(uint32_t *tag_base);
char** split_string(char *line, char **list);
int number_of_words(char *line);
int string_to_unsigned_int(char *input, int base);
int hex_value_of_character(char c);
