#include "streams.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
#include <stdint.h>
//#include <pthread.h>
#include "klibc.h"
#include "hashtable.h"

short initialized;
struct ht *q_table;
struct queue *q_map[100];
int_least32_t next;

void q_create(char q_name[], char options[])
{
	struct queue *q;
	os_memset(&q, 0, sizeof(q));
	if (!initialized) {
		initialized = 1;
        q_table = ht_alloc(100);
        ht_add(q_table, q_name, (void*)q);
	}else{
        ht_add(q_table, q_name, (void*)q);
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

int_least32_t q_open(char q_name[])
{
	if (!initialized) {
        return 0x0;
	}
	
    struct queue *result = ht_get(q_table, q_name);
    result->qd = result;
    q_map[next] = result;
    int_least32_t qd = next;
    next ++;//how should we deal with deleted streams? or should there just be a queue of x streams where we just delete the old ones?
    
	return qd;
}

int_least32_t q_publish(int_least32_t qd, int_least32_t *data, int_least32_t datalen)
{
	int kilo = 1024;
	if (datalen > 128 * kilo) {
		//TODO: throw an error
        return 0x0;
	}

	// TODO: Copy the data? Don't copy the data? Copy-on-write the data?
	struct queue *q = q_map[qd];
    q->data = data;
	q->datalen = datalen;
	return 0;
}

int_least32_t q_subscribe(int_least32_t qd, void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength), int_least32_t *userdata)
{
	////TODO: execute asynchronous callback with *receiver
    struct subscriber *user;
    os_memset(&user, 0, sizeof(user));
    user->userdata = userdata;
    //struct queue *q = ht_get(q_table, qd);

  
    // pthread_cond_signal(&q->update);//something like this?
//    (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength);
    //rcvr;
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
