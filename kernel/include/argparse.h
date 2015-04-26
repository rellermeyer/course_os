#ifndef __ARGPARSE_H__
#define __ARGPARSE_H__



#define ATAG_NONE	0x00000000
#define ATAG_CORE	0x54410001

struct atag_core {
        uint32_t flags; 
        uint32_t pagesize; 
        uint32_t rootdev; 
};

#define ATAG_MEM	0x54410002

struct atag_mem {
        uint32_t size; 
        uint32_t start;
};

#define ATAG_VIDEOTEXT	0x54410003
#define ATAG_RAMDISK	0x54410004
#define ATAG_INITRD2	0x54420005
#define ATAG_SERIAL	0x54410006
#define ATAG_REVISION	0x54410007
#define ATAG_VIDEOLFB	0x54410008
#define ATAG_CMDLINE	0x54410009

struct atag_cmdline {
        char    cmdline[1];
};

struct atag_header {
  uint32_t size;
  uint32_t tag;
};

struct atag {
        struct atag_header header;
        union {
                struct atag_core         core;
                struct atag_mem          mem;
                struct atag_cmdline      cmdline;
        } content;
};

static inline struct atag *atag_next_header(struct atag *t) 
{
   return (struct atag*) ((uint32_t*)(t) + t->header.size);  
}

#define atag_iterator(tag, addr) struct atag* tag = (struct atag*) addr; tag->header.tag != ATAG_NONE; tag = atag_next_header(tag)

void atag_print(struct atag *t);

void argparse_process(uint32_t *p_bootargs);
void parse_arguments(int argc, char **argv);
int analyze_arguments(char **argv);
char* read_cmdline_tag(uint32_t *tag_base);
char** split_string(char *line, char **list);
int number_of_words(char *line);
int string_to_unsigned_int(char *input, int base);
int hex_value_of_character(char c);

#endif
