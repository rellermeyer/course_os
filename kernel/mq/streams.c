#include "streams.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

void q_create(char q_name[], char options[])
{

}
int_least32_t q_open(char q_name[])
{

}
int_least32_t q_publish(int_least32_t qd, int_least32_t *data, int_least32_t datalen)
{

}
int_least32_t receiver(int_least32_t *userdata, int_least32_t *data, int_least32_t datalen)
{

}
int_least32_t q_subscribe(int_least32_t q, void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength), int_least32_t *userdata)
{

}



/*possibly to draw upon below methods later*/
//int_least32_t create_buffer(Stream *bs, int_least32_t length)
//{
//    if (bs == NULL)
//    {
//        return -1;
//    }
//    
//    bs->data = malloc(length);
//    memset(bs->data, 0, length);
//
//    bs->length = length;
//    bs->offset = 0;
//    return 0;
//}
//
//int_least32_t remaining_bytes(Stream *bs)
//{
//    return bs->length - bs->offset;
//}
//
//void clear(Stream *bs)
//{
//    bs-> offset = 0;
//}
//
//int_least32_t put_ele(Stream *bs, int_least32_t ele)
//{
//     if (remaining_bytes(bs) < 1)
//         return -1;
//     bs->data[bs->offset] = ele;
//     bs->offset ++;
//}
//
//int_least32_t get_ele(Stream *bs, int_least32_t loc)
//{
//    if (loc >= (bs->offset))
//        return -1;
//    return bs->data[loc];
//}

int main()
{
    Stream s;
    memset(&s, 0, sizeof(s));
    
  //  create_buffer(&s, 10);

}
