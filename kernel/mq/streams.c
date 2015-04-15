#include "streams.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
#include <stdint.h>
#include "klibc.h"

struct queue *head;
struct queue *tail;
short initialized;

void q_create(char q_name[], char options[])
{
	struct queue *q;
	os_memset(&q, 0, sizeof(q));
	if (!initialized) {
		// Hrm...
		/*initialized = 1;
		*head = *q;
		*tail = *q;*/
	}
	if (!os_strcmp(options,"rr")) {
		q->options = options;
		os_printf("printing rr\n");
	} else if(!os_strcmp(options,"broadcast")) {
		q->options = options;
		os_printf("printing broadcast\n");
	} else {
		os_printf("Error: options was %s. Expected \"rr\" or \"broadcast\"\n", options);
	}
}

struct queue *q_open(char q_name[])
{
	//TODO: is there a way to use a hashset or some variable arraylist of queues?
	if (!initialized) {
		//TODO: how do I handle errors?
	}
	struct queue *temp = head;
	while (temp->next != 0x0) {
		if (!os_strcmp(temp->q_name, q_name))
			return temp;
		temp = temp->next;
	}
	//TODO: No match found. Return error.
	return 0x0;
}

int_least32_t q_publish(/*int_least32_t qd --changed to queue for now*/ struct queue *q, int_least32_t *data, int_least32_t datalen)
{
	int kilo = 1024;
	if (datalen > 128 * kilo) {
		//TODO: throw an error
	}
	// TODO: Copy the data? Don't copy the data? Copy-on-write the data?
	q->data = data;
	q->datalen = datalen;
	return 0;
}

int_least32_t receiver(int_least32_t *userdata, int_least32_t *data, int_least32_t datalen)
{
	return 0;
}

int_least32_t q_subscribe(int_least32_t q, void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength), int_least32_t *userdata)
{
	//TODO: execute asynchronous callback with *receiver
	return 0;
}

void q_test()
{

	os_printf("***** Test code for message queue (q_test()): *****\n");
	os_printf("haha, as if we had tests. ;)\n");
	os_printf("***************************************************\n");
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

/*int main()
{
    Stream s;
    memset(&s, 0, sizeof(s));
    q_create("someArg", "rr");
  //  create_buffer(&s, 10);

}*/
