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
    void (*receiver)(uint32_t src_tid, uint32_t event, char * data, int length);
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
uint32_t receiver(uint32_t src_tid, uint32_t event, char * data, int length);
uint32_t q_subscribe(uint32_t q, void (*receiver)(uint32_t src_tid, uint32_t event, char * data, int length), void *userdata);
void q_send(uint32_t qd, void *data, uint32_t datalength);
void q_send_through_scheduler(uint32_t qd, void *data, uint32_t datalength);
uint32_t q_block_read(uint32_t qd, uint32_t *buf, uint32_t buflength);
void q_send_reply(uint32_t reply_qd, uint32_t *buf, uint32_t buflength);
void q_subscribe_to_reply(uint32_t reply_qd, void (*receiver)(uint32_t src_tid, uint32_t event, char * data, int length));
void q_reply_send(uint32_t reply_qd, void* data, uint32_t data_length);
void q_send_reply_through_scheduler(uint32_t reply_qd, uint32_t *buf, uint32_t buflength);
void sample_receiver(uint32_t src_tid, uint32_t event, char * data, int length);
void q_call(char q_name[], void *data, void (*receiver)(uint32_t src_tid, uint32_t event, char * data, int length), void *userdata);
void q_init(char q_name[], void* data, void(*receiver)(uint32_t src_tid, uint32_t event, char * data, int length), void* userdata);
void q_test();
/*to add other functions below*/



/*possibly draw some ideas from below functions*/
//uint32_t create_buffer(Stream *bs, uint32_t lenth);
//uint32_t remaining_bytes(Stream *bs);
//void clear(Stream *bs);
//uint32_t put_element(Stream* bs, uint32_t ele);
//uint32_t get_element(Stream* bs, uint32_t loc);
#endif
