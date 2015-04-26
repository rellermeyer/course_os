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
    void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength);
    struct queue *next;
};

struct subscriber
{
    int_least32_t *userdata;
//    int_least32_t *next;//implement later if add multi- subscribers
};

void q_create(char q_name[]/*, char options[]*/);
int_least32_t q_open(char q_name[]);
int_least32_t q_publish(int_least32_t qd, int_least32_t *data, int_least32_t datalen);
int_least32_t receiver(int_least32_t *userdata, int_least32_t *data, int_least32_t datalen);
int_least32_t q_subscribe(int_least32_t q, void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength), int_least32_t *userdata);
int_least32_t q_block_read(int_least32_t qd, int_least32_t *buf, int_least32_t buflength);
void q_wait_for_reply(char msg[], int_least32_t *buf, int_least32_t buflength);
void q_subscribe_to_reply(char msg[], void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength));
void sample_receiver(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength);
void q_call(char q_name[], int_least32_t *data, void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength), int_least32_t *userdata);
void q_test();
/*to add other functions below*/



/*possibly draw some ideas from below functions*/
//int_least32_t create_buffer(Stream *bs, int_least32_t lenth);
//int_least32_t remaining_bytes(Stream *bs);
//void clear(Stream *bs);
//int_least32_t put_element(Stream* bs, int_least32_t ele);
//int_least32_t get_element(Stream* bs, int_least32_t loc);
#endif
