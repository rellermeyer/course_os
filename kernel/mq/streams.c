#include "streams.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
#include <stdint.h>
#include "klibc.h"
#include "hashtable.h"
#include "drivers/uart.h"

static int initialized;
struct ht *q_table;
struct queue *q_map[5000];
int_least32_t next;

void q_create(char q_name[]/*, char options[]*/)
{
	struct queue *q = 0x0;
    if (initialized < 0){
        initialized = 0;
    }

	if (initialized == 0) {
		initialized = 1;
		q_table = ht_alloc(5000);
		ht_add(q_table, q_name, (void*)q);
	}else{
		ht_add(q_table, q_name, (void*)q);
	}



    //implement the below if we increase # of subscribers
//	if (!os_strcmp(options,"rr")) {
//		q->options = options;
//            os_printf("printing rr\n");
//	} else if(!os_strcmp(options,"broadcast")) {
//		q->options = options;
//		os_printf("printing broadcast\n");
//	} else {
//		os_printf("Error: options was %s. Expected \"rr\" or \"broadcast\"\n", options);
//	}
}

int_least32_t q_open(char q_name[])
{
	if (initialized == 0) {
        return 0x0;
	}

    //change this hack. Why isn't it initialized to 0 already?
    if (next < 0 || next > 5000){
        next = 0;
    }
    struct queue *result = ht_get(q_table, q_name);
    result->qd = result;
    q_map[next] = result;
    int_least32_t qd = next;
    next++;//how should we deal with deleted streams? or should there just be a queue of x streams where we just delete the old ones?
    
	return qd;
}
// Adding message to queue
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
    //Accessing elements of q throws an error
	return 0;
}

int_least32_t q_subscribe(int_least32_t qd, void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength), int_least32_t *userdata)
{
    struct subscriber *user = 0x0;
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

// block, waiting for a message from the queue
// read entire message or no data at all
int_least32_t q_block_read(int_least32_t qd, int_least32_t *buf, int_least32_t buflength)
{
    struct queue *current_queue = q_map[qd];
    // wait until the queue delivers a message
    // since NULL is undefined in the kernel, use 0x0 instead
    // need to use condition variables
    while (current_queue->data == 0x0)
        // BLOCK!
    
    // check to see if data length is acceptable
    if (current_queue->datalen <= buflength) {
        // read data
        buf = current_queue->data;
        // success
        return 1;
    }
    // data too big
    os_printf_v2("Writing to buffer failed\n");
    return 0;
}

// waits for reply, and when the reply comes, fills the buffer with it. Fails if buffer length too small
void q_wait_for_reply(char msg[], int_least32_t *buf, int_least32_t buflength)
{
    // why char msg[]? unlike int qd
    // block while waiting for reply (condition variable)
    struct queue *reply_q = q_map[msg[0]];
    // success data fits into buffer
    if (reply_q->datalen <= buflength) {
        buf = reply_q->data;
        reply_q->receiver(reply_q->subscriber->userdata, reply_q->data, reply_q->datalen);
    }
    // data is too big for buffer
    else
        os_printf_v2("Writing to buffer failed\n");
}

// opposite of q_send and q_subscribe, attaches an asynchronous receiver to the reply
void q_subscribe_to_reply(char msg[], void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength))
{

}

/*q_call seems to fail on q_send*/
//void q_call(char q_name[], int_least32_t* data, void(*receiver)(int_least32_t * userdata, int_least32_t * data, int_least32_t datalength), int_least32_t* userdata)
//{
//    q_create(q_name);
//    int_least32_t qd = q_open(q_name);
//    q_publish(qd, (int_least32_t*) data, sizeof(data));
//    q_subscribe(qd, &receiver, (int_least32_t*) userdata);
//    q_send(qd, (int_least32_t*) data, sizeof(data));
//}

void q_init(char q_name[], int_least32_t* data, void(*receiver)(int_least32_t * userdata, int_least32_t * data, int_least32_t datalength), int_least32_t* userdata)
{
    q_create(q_name);
    int_least32_t qd = q_open(q_name);
    q_publish(qd, (int_least32_t*) data, sizeof(data));
    q_subscribe(qd, &receiver, (int_least32_t*) userdata);
}

void sample_receiver(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength)
{
	print_uart0("foobar.\n");
	//print_uart0("data: %s\n", data);
}

void q_test()
{
    os_printf_v2("this is a test\n");
    q_create("my_q");
    os_printf_v2("Calling q_open...\n");
    int_least32_t qd = q_open("my_q");
    os_printf_v2("qd is %d\n", qd);
    char* data = "This is a message";
    int_least32_t *castData = (int_least32_t*) data;
    q_publish(qd, castData, sizeof(data));
    char* userdata = "Some userdata";
    int_least32_t *castUserdata = (int_least32_t*) userdata;// what should this really be?
    void *fp = &sample_receiver;
    //fp += 0x10000;
    //os_printf("Function pointer is: %X\n", fp);
    os_printf_v2("First 4 bytes at %X are: %X\n", fp, *(uint32_t*)fp);
    q_subscribe(qd, (void (*)(int_least32_t*, int_least32_t*, int_least32_t))fp, castUserdata);
    os_printf_v2("asdf\n");
    q_send(qd, castData, sizeof(data));
}


