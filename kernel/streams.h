#include <stdint.h>

typedef struct
{
    int_least32_t *data;
    int_least32_t length;
    int_least32_t offset;
} Stream;

int_least32_t create_buffer(Stream *bs, int_least32_t lenth);
int_least32_t remaining_bytes(Stream *bs);
void print_buffer(Stream *bs);

