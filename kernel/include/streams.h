#ifndef __streams_h
#define __streams_h
#include <stdint.h>

typedef struct
{
    int_least32_t *data;
    int_least32_t length;
    int_least32_t offset;
} Stream;

typedef struct
{
    int_least32_t *data;
    int_least32_t datalen;
    char *q_name;
    char *options;
    struct Queue *next;
}Queue;

void q_create(char q_name[], char options[]);
Queue q_open(char q_name[]);
int_least32_t q_publish(Queue *q, int_least32_t *data, int_least32_t datalen);
int_least32_t receiver(int_least32_t *userdata, int_least32_t *data, int_least32_t datalen);
int_least32_t q_subscribe(int_least32_t q, void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength), int_least32_t *userdata);
/*to add other functions below*/



/*possibly draw some ideas from below functions*/
//int_least32_t create_buffer(Stream *bs, int_least32_t lenth);
//int_least32_t remaining_bytes(Stream *bs);
//void clear(Stream *bs);
//int_least32_t put_element(Stream* bs, int_least32_t ele);
//int_least32_t get_element(Stream* bs, int_least32_t loc);
#endif
