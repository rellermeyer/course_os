#include "streams.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int_least32_t create_stream(Stream *bs, int_least32_t length)
{
    if (bs == NULL)
    {
        return -1;
    }
    
    bs->data = malloc(length);
    memset(bs->data, 0, length);

    bs->length = length;
    bs->offset = 0;
    return 0;
}

int main()
{

}
