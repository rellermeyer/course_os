#ifndef __streams_h
#define __streams_h
#include <stdint.h>

//struct stream
//{
//	void *data;
//	uint32_t length;
//	uint32_t offset;
//};

struct queue
{
	void *data;
	uint32_t datalen;
	char *q_name;
    struct queue *qd;
	char *options;
	struct subscriber *subscriber;
    //struct subscriber *subscriber_head;//to implement if change to multi- subscriber model
    //struct subscriber *subscriber_tail;
    void (*receiver)(void *userdata, void *data, uint32_t datalength);
    struct queue *next;
};

struct subscriber
{
    void *userdata;
//    uint32_t *next;//implement later if add multi- subscribers
};

void q_create(char q_name[]/*, char options[]*/);
uint32_t q_open(char q_name[]);
uint32_t q_publish(uint32_t qd, void *data, uint32_t datalen);
uint32_t receiver(void *userdata, void *data, uint32_t datalen);
uint32_t q_subscribe(uint32_t q, void (*receiver)(void *userdata, void *data, uint32_t datalength), void *userdata);
uint32_t q_block_read(uint32_t qd, uint32_t *buf, uint32_t buflength);
void q_wait_for_reply(uint32_t reply_qd, uint32_t *buf, uint32_t buflength);
void q_subscribe_to_reply(uint32_t reply_qd, void (*receiver)(void *userdata, void *data, uint32_t datalength));
void sample_receiver(void *userdata, void *data, uint32_t datalength);
void q_call(char q_name[], void *data, void (*receiver)(void *userdata, void *data, uint32_t datalength), void *userdata);
void q_init(char q_name[], void* data, void(*receiver)(void * userdata, void * data, uint32_t datalength), void* userdata);
void q_test();
/*to add other functions below*/



/*possibly draw some ideas from below functions*/
//uint32_t create_buffer(Stream *bs, uint32_t lenth);
//uint32_t remaining_bytes(Stream *bs);
//void clear(Stream *bs);
//uint32_t put_element(Stream* bs, uint32_t ele);
//uint32_t get_element(Stream* bs, uint32_t loc);
#endif
