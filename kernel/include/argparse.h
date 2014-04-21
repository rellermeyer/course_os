#include <stdint.h>

void parse_arguments(int argc, char **argv);
int analyze_args(char **argv);
char* read_cmdline_tag(uint32_t *tag_base);
char** split_string(char *line, char **list);
int number_of_words(char *line);
