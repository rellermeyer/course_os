#ifndef __streams_h
#define __streams_h
#include <stdint.h>

//struct stream
//{
//	int_least32_t *data;
//	int_least32_t length;
//	int_least32_t offset;
//};

struct queue
{
	int_least32_t *data;
	int_least32_t datalen;
	char *q_name;
    struct queue *qd;
	char *options;
	struct subscriber *subscriber;
    //struct subscriber *subscriber_head;//to implement if change to multi- subscriber model
    //struct subscriber *subscriber_tail;
    struct queue *next;
};

struct subscriber
{
    int_least32_t *userdata;
//    int_least32_t *next;
};

void q_create(char q_name[], char options[]);
int_least32_t q_open(char q_name[]);
int_least32_t q_publish(int_least32_t qd, int_least32_t *data, int_least32_t datalen);
int_least32_t receiver(int_least32_t *userdata, int_least32_t *data, int_least32_t datalen);
int_least32_t q_subscribe(int_least32_t q, void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength), int_least32_t *userdata);
void q_test();
/*to add other functions below*/



/*possibly draw some ideas from below functions*/
//int_least32_t create_buffer(Stream *bs, int_least32_t lenth);
//int_least32_t remaining_bytes(Stream *bs);
//void clear(Stream *bs);
//int_least32_t put_element(Stream* bs, int_least32_t ele);
//int_least32_t get_element(Stream* bs, int_least32_t loc);
#endif
