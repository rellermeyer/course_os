#include "streams.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
#include <stdint.h>
#include "klibc.h"
#include "hashtable.h"
#include "drivers/uart.h"
#include "scheduler.h"

/*only call scheduler stuff(emit, broadcast, wait, sched_yeild), not kthr
 * do x = false whenever data comes in in test_kthread
 * use sched_yield in while loop instead of interupt which gives other threads chance to run
 * */
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
    print_uart0("initialized queue\n");
    if (initialized != 0){
        initialized = 0;

        q_table = ht_alloc(5000);
        ht_add(q_table, q_name, (void*)q);
    }else{
        ht_add(q_table, q_name, (void*)q);
    }
//
    //implement the below if we increase # of subscribers
//  if (!os_strcmp(options,"rr")) {
//      q->options = options;
//  } else if(!os_strcmp(options,"broadcast")) {
//      q->options = options;
//  } else {
//  }
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
    // TODO: Copy the data? Don't copy the data? Copy-on-write the data?
    struct queue *q = q_map[qd];
    q->data = data;
    q->datalen = datalen;
    //Accessing elements of q throws an error
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

uint32_t q_subscribe(uint32_t qd, void (*receiver)(uint32_t src_tid, uint32_t event, char * data, int length), void *userdata)
{
    struct subscriber *user = 0x0;
    user->userdata = userdata;
    struct queue *q = q_map[qd];
    q->subscriber = user;
    q->receiver = receiver; 
    return 0;
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
    
    //print_uart0("in q send\n");
    uint32_t spaceAvail = 1028;
    struct queue *q = q_map[qd];
    while (datalength > spaceAvail) {
        print_uart0("in while loop\n");
        q->receiver(1, 2, startingPoint, spaceAvail);
        startingPoint = (uint32_t)&startingPoint + spaceAvail;
        datalength = datalength - spaceAvail;
    } 
    if(datalength > 0){
        //q->subscriber->userdata
        q->receiver(1, 2, startingPoint, datalength);
    }
    //TODO: delete data after sending
    
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
    void *startingPoint = data;
    
    uint32_t spaceAvail = sched_get_message_space();
    struct queue *q = q_map[qd];
    while (datalength > spaceAvail) {
        print_uart0("in while loop\n");
        q->receiver(1, 2, startingPoint, spaceAvail);
        startingPoint = (uint32_t)&startingPoint + spaceAvail;
        datalength = datalength - spaceAvail;
    } 
    if(datalength > 0){
        //q->subscriber->userdata
        q->receiver(1, 2, startingPoint, datalength);
    }
}

/**
 * block while waiting for the message from the queue
 * either succeed or fail, reading entire message with buffer or no data at all
 *
 * This function is incomplete. We want to block while waiting for the mssage from the queue.
 * When the message is successfully added onto the queue, read the data using the buffer.
 *
 * @param qd represents a queue descriptor
 * @param buf represents the buffer that will be used to read the data
 * @param buflength represents the length of the buffer
 *
 * @return the data in the queue or NULL if the data is too big for the ubffer
 */

uint32_t q_block_read(uint32_t qd, uint32_t *buf, uint32_t buflength)
{
    struct queue *current_queue = q_map[qd];
    // // since NULL is undefined in the kernel, use 0x0 instead
    // while (current_queue->data == 0x0)

    
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


/**
 * Reply with the data using the given queue
 *
 * This function enables the recipient to reply and send the data.
 * This function breaks data into chunks of size 512 bytes in order to prevent hanging on too large of data.
 *
 * @param reply_qd represents the queue descriptor for the reply queue
 * @param data represents the actual data being passed
 * @param datalen represents the length of the data. 
 *
 * @return this function does not return data
 */

void q_send_reply(uint32_t reply_qd, uint32_t *data, uint32_t datalength)
{
    struct queue *reply_q = q_map[reply_qd];
    //get exact available space
    void *startingPoint = data;
    
    //print_uart0("in q send\n");
    uint32_t spaceAvail = 1028;
    while (datalength > spaceAvail) {
        print_uart0("in while loop\n");
        reply_q->receiver(1, 2, startingPoint, spaceAvail);
        startingPoint = (uint32_t)&startingPoint + spaceAvail;
        datalength = datalength - spaceAvail;
    } 
    if (datalength > 0){
        //q->subscriber->userdata
        reply_q->receiver(1, 2, startingPoint, datalength);
    }
}



/**
 * Reply with the data using the given queue with the scheduler
 *
 * This function enables the recipient to reply and send the data.
 * Allows to check how much message space is left.
 * This function breaks data into chunks of size 512 bytes in order to prevent hanging on too large of data.
 *
 * @param reply_qd represents the queue descriptor for the reply queue
 * @param data represents the actual data being passed
 * @param datalen represents the length of the data. 
 *
 * @return this function does not return data
 */

void q_send_reply_through_scheduler(uint32_t reply_qd, uint32_t *data, uint32_t datalength)
{
    struct queue *reply_q = q_map[reply_qd];
    //get exact available space
    void *startingPoint = data;
    
    //print_uart0("in q send\n");
    uint32_t spaceAvail = sched_get_message_space();
    while (datalength > spaceAvail) {
        print_uart0("in while loop\n");
        reply_q->receiver(1, 2, startingPoint, spaceAvail);
        startingPoint = (uint32_t)&startingPoint + spaceAvail;
        datalength = datalength - spaceAvail;
    } 
    if (datalength > 0){
        //q->subscriber->userdata
        reply_q->receiver(1, 2, startingPoint, datalength);
    }
}


/**
 * Subscribes to the reply queue
 *
 * Given the reply queue descriptor, allow user to subscribe to the reply queue.
 * This allow the recipient to send the subscribers by sending to the receiver (the callback handler).
 *
 * @param reply_qd represents a reply queue descriptor
 * @param receiver represents a callback handler function pointer that performs the given operation of the stream.
 *
 * @return this function does not return data
 */

void q_subscribe_to_reply(uint32_t reply_qd, void (*receiver)(uint32_t src_tid, uint32_t event, char * data, int length))
{
   struct queue *reply_q = q_map[reply_qd];
   reply_q->receiver = receiver;
}


// void q_init(char q_name[], void* data, void(*receiver)(uint32_t src_tid, uint32_t event, char * data, int length), void* userdata)
// {
//     q_create(q_name);
//     uint32_t qd = q_open(q_name);
//     q_publish(qd, (uint32_t*) data, sizeof(data));
//     q_subscribe(qd, &receiver, (void*) userdata);
// }

void sample_receiver(uint32_t src_tid, uint32_t event, char * data, int length)
{
    //print_uart0("In Sample receiver. this is registered in scheduler.\n");
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
    os_printf("In child_receiver. parent_tid is %d tid is %d\n", parent_tid, tid);

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
    os_printf("in parent_reciever. parent_tid is %d tid is %d\n", parent_tid, tid);
    sched_register_callback_handler(&sample_receiver);
    os_printf("in parent receiver. done scheduling\n");
    kthr_start(kthr_create(&child_receiver));
    os_printf("in parent_reciever. started child thread\n");
    kthr_yield();
    os_printf("in parent_reciever. done yielding\n");
}

void q_test()
{
    // test client
    os_printf("this is a test\n");
    print_uart0("in q_test\n");
    q_create("printf");
	int qd = q_open("printf");
	q_subscribe(qd, sample_receiver, 0x0);
    os_printf("msg is %d\n", q_publish(qd, "first message", 13));
    // struct queue *test_q = q_map[qd];
    // // check client message
    // os_printf("%s\n", (char*) test_q->data);
    // uint32_t number = 0;
    // uint32_t *sample_buffer = &number;
    // q_create("printf_2");
    //int reply_qd = q_open("printf_2");
    //q_block_read(qd, *sample_buffer, 100);
    // test buffer
    //os_printf("%s\n", (char*) sample_buffer);



    char* mydata = "yo, dawg\n";
    q_create("message");
    qd = q_open("message");
    q_subscribe(qd, sample_receiver, 0x0);
    sched_init();
    kthr_start(kthr_create(&parent_reciever));
    kthr_yield();
    q_send(qd, mydata, sizeof(mydata));
    mydata = "sup g\n";
    q_send(qd, mydata, sizeof(mydata));

    q_create("reply");
    int reply_qd = q_open("reply");
    q_subscribe_to_reply(reply_qd, &child_receiver);


    // kthr_yield();
    //char* word = "hello dog";
   // sched_post_message(1, 2, word, sizeof(word));
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
