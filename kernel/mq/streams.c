#include "streams.h"
#include <stdint.h>
#include "klibc.h"
#include "hashtable.h"
#include "drivers/uart.h"
#include "scheduler.h"


static int initialized;
struct ht *q_table;
struct queue *q_map[5000];
uint32_t next;

/**
 *Creates a queue
 *
 * Q_create creates allows a client to create a queue using a character
 * description as q_name. This function uses a hashtable to map the q_name to
 * the physical address of the queue.
 *
 * @param q_name represents a new queue by name
 *
 * @return does not return a value;
 */

void q_create(char q_name[])
{
    struct queue *q = 0x0;
    if (initialized != 0){
        initialized = 0;

        q_table = ht_alloc(5000);
        ht_add(q_table, q_name, (void*)q);
    }else{
        ht_add(q_table, q_name, (void*)q);
    }
}

/**
 * Opens a queue
 *
 * Given a q_name, q_create uses q_map to fetch a queue from memory
 * and proceed to map that queue to a q descriptor using an array of pointers.
 * This provides greater efficiency and usability. This function also returns 
 * a queue descriptor that can be used to identify the queue when subscribing and sending.
 * 
 * @param q_name represents a new queue by name
 *
 * @return returns a queue descriptor
 */

uint32_t q_open(char q_name[])
{
    if (initialized != 0) {
        return 0x0;
    }

    if (next < 0 || next > 5000){
        next = 0;
    }
    struct queue *result = ht_get(q_table, q_name);
    result->isStreaming = 0;
    result->qd = result;
    q_map[next] = result;
    uint32_t qd = next;
    next++;
    return qd;
}

/**
 * Publishes to a queue
 *
 * Adds data to a queue to be used at a later point for the
 * passing of data. Can be used in a more lazy fashion than q_send.
 *
 * @param qd represents a q descriptor
 * @param data represents the actual data being transferred
 * @param datalen represents the length of the data
 *
 * @return returns the address of the data
 */

uint32_t q_publish(uint32_t qd, void *data, uint32_t datalen)
{
    struct queue *q = q_map[qd];
    q->data = data;
    q->datalen = datalen;
    return &(q->data);
}

/**
 * Subscribes to a queue
 *
 * Allows users to subscribe to a particular queue by using the q_descriptor 
 * received from q-open. This allows the client to send the the subscribers
 * by sending to the registred callback function. Note that in future implmentations,
 * there could be multiple subscribers to a particular queue, but for efficiency,
 * we limited this to one subscriber for now.
 *
 * @param qd represents a q descriptor
 * @param receiver represents a callback handler function pointer that performs the given operation of the stream.
 * @param userdata is some arbitrary identifier given to the client or user. 
 *
 * @return this function does not return data
 */

void q_subscribe(uint32_t qd, void (*receiver)(uint32_t src_tid, uint32_t event, char * data, int length), void *userdata)
{
    struct subscriber *user = 0x0;
    user->userdata = userdata;
    struct queue *q = q_map[qd];
    q->subscriber = user;
    q->receiver = receiver; 
}

/**
 * Sends data using a given queue
 *
 * This function allows a user to send data that is transmitted by 
 * the callback handler registered in q_subscribe. This function can be
 * called within the client code (e.g. printf) in order to perform the desired message passing.
 * This function breaks data into chunks of size 512 bytes in order to prevent hanging on too large of data.
 *
 * @param qd represents a q descriptor
 * @param data represents the actual data being passed
 * @param datalen represents the length of the data. 
 *
 * @return this function does not return data
 */

void q_send(uint32_t qd, void *data, uint32_t datalength)
{
    //get exact available space
    void *startingPoint = data;
    uint32_t spaceAvail = 512;
    struct queue *q = q_map[qd];
    while (datalength > spaceAvail) {
        print_uart0("in while loop\n");
        q->receiver(1, 2, startingPoint, spaceAvail);
        startingPoint = (uint32_t)&startingPoint + spaceAvail;
        datalength = datalength - spaceAvail;
    } 
    if(datalength > 0){
        q->receiver(1, 2, startingPoint, datalength);
    }    
}

/**
 * Sends data using a given queue using the scheduler
 *
 * This function allows a user to send data that is transmitted by 
 * the callback handler registered in q_subscribe. This function can be
 * called within the client code (e.g. printf) in order to perform the desired message passing.
 * This function breaks data into chunks based on the size specified by the scheduler
 * in order to prevent hanging on too large of data for the scheduler.
 *
 * @param qd represents a q descriptor
 * @param data represents the actual data being passed
 * @param datalen represents the length of the data. 
 *
 * @return this function does not return data
 */

void q_send_through_scheduler(uint32_t qd, void *data, uint32_t datalength) {
    //get exact available space
    struct queue *q = q_map[qd];
    if(! q->isStreaming){//checks if data is still being transmitted
        q->startingPoint = data;
    }
    q->isStreaming = 1;
    
    uint32_t spaceAvail = sched_get_message_space();
    
    while (datalength > spaceAvail) {
        q->receiver(1, 2, q->startingPoint, spaceAvail);
        q->startingPoint = (uint32_t)&(q->startingPoint) + spaceAvail;
        datalength = datalength - spaceAvail;
    } 
    if(datalength > 0){
        q->receiver(1, 2, q->startingPoint, datalength);
    }
    q->isStreaming = 0;
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
void q_send_reply(uint32_t reply_qd, uint32_t *data, uint32_t datalength)
{
    struct queue *reply_q = q_map[reply_qd];
    //get exact available space
    void *startingPoint = data;
    
    uint32_t spaceAvail = 512;
    while (datalength > spaceAvail) {
        print_uart0("in while loop\n");
        reply_q->receiver(1, 2, startingPoint, spaceAvail);
        startingPoint = (uint32_t)&startingPoint + spaceAvail;
        datalength = datalength - spaceAvail;
    } 
    if (datalength > 0){
        reply_q->receiver(1, 2, startingPoint, datalength);
    }
}


// waits for reply, and when the reply comes, fills the buffer with it. Fails if buffer length too small
void q_send_reply_through_scheduler(uint32_t reply_qd, uint32_t *data, uint32_t datalength)
{
    struct queue *reply_q = q_map[reply_qd];
    if(! reply_q->isStreaming){//checks if data is still being transmitted
        reply_q->startingPoint = data;
    }
    reply_q->isStreaming = 1;
    
    uint32_t spaceAvail = sched_get_message_space();
    while (datalength > spaceAvail) {
        print_uart0("in while loop\n");
        reply_q->receiver(1, 2, reply_q->startingPoint, spaceAvail);
        reply_q->startingPoint = (uint32_t)&(reply_q->startingPoint) + spaceAvail;
        datalength = datalength - spaceAvail;
    } 
    if (datalength > 0){
        reply_q->receiver(1, 2, reply_q->startingPoint, datalength);
    }

    reply_q->isStreaming = 0;
}


// attaches an asynchronous receiver to the reply
void q_subscribe_to_reply(uint32_t reply_qd, void (*receiver)(uint32_t src_tid, uint32_t event, char * data, int length))
{
    print_uart0("entering subscr to reply\n");
   struct queue *reply_q = q_map[reply_qd];
   reply_q->receiver = receiver;
   print_uart0("exiting subscr to reply\n");
}


void sample_receiver(uint32_t src_tid, uint32_t event, char * data, int length)
{
    int i;
    char *s = (char*)data;
    for (i=0; i<length; i++) {
        print_char_uart0(s[i]);
    }
}



void child_receiver(uint32_t parent_tid, uint32_t tid)
{
    char * msg = "-------------sent from child-------------\n";
    uint32_t msg_len = os_strlen(msg);
    uint32_t event_id = 1;

    if(sched_get_message_space() >= msg_len){
        if(sched_post_message(parent_tid, event_id, msg, msg_len)){
            ERROR("Message fail\n");
        }else{
            os_printf("child_receiver. MESSAGE PASSED\n");
        }
    }
}

void parent_reciever(uint32_t parent_tid, uint32_t tid)
{
    sched_register_callback_handler(&sample_receiver);
    kthr_start(kthr_create(&child_receiver));
    kthr_yield();
}

void q_test()
{
    // test client
    os_printf("PASSES OS_PRINTF\n");
    q_create("printf");
	int qd = q_open("printf");
	q_subscribe(qd, sample_receiver, 0x0);


    char* mydata = "PASSES: prints message 1\n";
    q_create("message");
    qd = q_open("message");
    q_subscribe(qd, sample_receiver, 0x0);
    sched_init();
    kthr_start(kthr_create(&parent_reciever));
    kthr_yield();
    q_send(qd, mydata, os_strlen(mydata));
    mydata = "PASSES: prints message 2\n";
    q_send(qd, mydata, os_strlen(mydata));


    q_subscribe(qd, printf_receiver, 0x0);
    q_create("reply");
    int reply_qd = q_open("reply");
    q_subscribe_to_reply(reply_qd, &child_receiver);
    q_subscribe(qd, printf_receiver, 0x0);

    return TEST_OK;

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
