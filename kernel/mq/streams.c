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
uint32_t next;

void q_create(char q_name[])
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
//	} else if(!os_strcmp(options,"broadcast")) {
//		q->options = options;
//	} else {
//	}
}

uint32_t q_open(char q_name[])
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
    uint32_t qd = next;
    next++;//how should we deal with deleted streams? or should there just be a queue of x streams where we just delete the old ones?
    
	return qd;
}
// Adding message to queue
uint32_t q_publish(uint32_t qd, void *data, uint32_t datalen)
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

uint32_t q_subscribe(uint32_t qd, void (*receiver)(void *userdata, void *data, uint32_t datalength), void *userdata)
{
    struct subscriber *user = 0x0;
    user->userdata = userdata;
	struct queue *q = q_map[qd];
    q->subscriber = user;
    q->receiver = receiver; 
    return 0;
}

void q_send(uint32_t qd, void *data, uint32_t datalength)
{

    struct queue *q = q_map[qd];
    q->receiver(q->subscriber->userdata, data, datalength);
}

// block, waiting for a message from the queue
// read entire message or no data at all
uint32_t q_block_read(uint32_t qd, uint32_t *buf, uint32_t buflength)
{
    struct queue *current_queue = q_map[qd];
    // since NULL is undefined in the kernel, use 0x0 instead
    // need to use condition variables
    while (current_queue->data == 0x0)
        // BLOCK! FIX
    
    // check to see if data length is acceptable
    if (current_queue->datalen <= buflength) {

        uint32_t buf_index = *buf;
        // read to buffer
        for (uint32_t data_index = 0; data_index < current_queue->datalen; data_index++) {
            buf[buf_index] = ((uint32_t*) current_queue->data)[data_index];
            buf_index++;
        }

        return ((uint32_t) current_queue->data);
    }
    // data too big
    return 0;
}

// waits for reply, and when the reply comes, fills the buffer with it. Fails if buffer length too small
void q_wait_for_reply(uint32_t reply_qd, uint32_t *buf, uint32_t buflength)
{
    // changed char msg[] to uint32_t reply_qd
    // block while waiting for reply (condition variable)
    struct queue *reply_q = q_map[reply_qd];
    // success data fits into buffer
    if (reply_q->datalen <= buflength) {
        uint32_t buf_index = *buf;
        // read to buffer
        for (uint32_t data_index = 0; data_index < reply_q->datalen; data_index++) {
            buf[buf_index] = ((uint32_t*) reply_q->data)[data_index];
            buf_index++;
        }
        reply_q->receiver(reply_q->subscriber->userdata, reply_q->data, reply_q->datalen);
    }
    // data is too big for buffer
    else{
    }
}

// attaches an asynchronous receiver to the reply
void q_subscribe_to_reply(uint32_t reply_qd, void (*receiver)(void *userdata, void *data, uint32_t datalength))
{
   struct queue *reply_q = q_map[reply_qd];
   reply_q->receiver = receiver;
}

// void q_reply_send(uint32_t reply_qd, void* data, uint32_t data_length) {
//     struct queue *reply_qd = q_map[reply_qd];
//     reply_qd->receiver(reply_qd->subscriber->userdata, data, data_length);
// }

/*q_call seems to fail on q_send*/
//void q_call(char q_name[], void * data, void(*receiver)(void * userdata, void * data, uint32_t datalength), void* userdata)
//{
//    q_create(q_name);
//    uint32_t qd = q_open(q_name);
//    q_publish(qd, (uint32_t*) data, sizeof(data));
//    q_subscribe(qd, &receiver, (void*) userdata);
//    q_send(qd, (uint32_t*) data, sizeof(data));
//}

void q_init(char q_name[], void* data, void(*receiver)(void * userdata, void * data, uint32_t datalength), void* userdata)
{
    q_create(q_name);
    uint32_t qd = q_open(q_name);
    q_publish(qd, (uint32_t*) data, sizeof(data));
    q_subscribe(qd, &receiver, (void*) userdata);
}

void sample_receiver(void *userdata, void *data, uint32_t datalength)
{
	int i;
	char *s = (char*)data;
	for (i=0; i<datalength; i++) {
		print_char_uart0(s[i]);
	}
}

void q_test()
{
    // test client
    q_create("printf");
	int qd = q_open("printf");
	q_subscribe(qd, sample_receiver, 0x0);
    q_publish(qd, "first message", 13);
    struct queue *test_q = q_map[qd];
    // check client message
    os_printf("%s\n", (char*) test_q->data);
    uint32_t number = 0;
    uint32_t *sample_buffer = &number;
    q_block_read(qd, *sample_buffer, 100);
    // test buffer
    os_printf("%s\n", (char*) sample_buffer);


	print_uart0("This is a test.\n");
	os_printf("This is a 2nd test.\n");
	return;

    // os_printf_v2("this is a test\n");
    // q_create("my_q");
    // os_printf_v2("Calling q_open...\n");
    // qd = q_open("my_q");
    // os_printf_v2("qd is %d\n", qd);
    // char* data = "This is a message";
    // uint32_t *castData = (uint32_t*) data;
    // q_publish(qd, castData, sizeof(data));
    // char* userdata = "Some userdata";
    // uint32_t *castUserdata = (void*) userdata;// what should this really be?
    // void *fp = &sample_receiver;
    // //fp += 0x10000;
    // //os_printf("Function pointer is: %X\n", fp);
    // os_printf_v2("First 4 bytes at %X are: %X\n", fp, *(uint32_t*)fp);
    // q_subscribe(qd, (void (*)(uint32_t*, uint32_t*, uint32_t))fp, castUserdata);
    // os_printf_v2("asdf\n");
    // q_send(qd, castData, sizeof(data));
}
