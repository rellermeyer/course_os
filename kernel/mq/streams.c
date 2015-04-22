#include "streams.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
#include <stdint.h>
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
	ht_add(q_table, q_name, (void*)q);
	if (!initialized) {
		initialized = 1;
        q_table = ht_alloc(100);      
    }
    //implement the below if we increase # of subscribers
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
    next++;//how should we deal with deleted streams? or should there just be a queue of x streams where we just delete the old ones?
    
	return qd;
}

int_least32_t q_publish(int_least32_t qd, int_least32_t *data, int_least32_t datalen)
{
	int kilo = 1024;
	if (datalen > 128 * kilo) {
		// TODO: throw an error
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
    struct subscriber *user;
    os_memset(&user, 0, sizeof(user));
    user->userdata = userdata;
	struct queue *q = q_map[qd];
    q->subscriber = user;
    q->receiver = receiver; 
    return 0;
}

void q_send(int_least32_t qd, int_least32_t *data, int_least32_t datalength)
{
    struct queue *q = q_map[qd];
    q->receiver(q->subscriber->userdata, data, datalength);
}

int_least32_t q_block_read(int_least32_t qd, int_least32_t *buf, int_least32_t buflength)
{
    
}

void q_wait_for_reply(char msg[], int_least32_t buf, int_least32_t bulength)
{
    
}

void q_subscribe_to_reply(char msg[], (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength))
{

}
void q_test()
{

	os_printf("***** Test code for message queue (q_test()): *****\n");
	os_printf("haha, as if we had tests. ;)\n");
	os_printf("***************************************************\n");
}


