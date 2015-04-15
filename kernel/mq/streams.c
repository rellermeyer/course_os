#include "../include/streams.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

Queue *head;
Queue *tail;
short initialized;

void q_create(char q_name[], char options[])
{
	Queue *q;
    memset(&q, 0, sizeof(q));
    if (!initialized){
        initialized = 1;
        *head = *q;
        *tail = *q;
    }else{
        tail->next = q;
        tail = q;
    }
    if (options == "rr"){
        q->options = options;
		printf("printing rr\n");
	}else if(options == "broadcast"){
        q->options = options;
		printf("printing broadcast\n");
	}else{
		printf("Error: options was %s. Expected \"rr\" or \"broadcast\"\n", options);
	}
}
Queue q_open(char q_name[])
{
    //TODO: Ask lane if need to provide alternative to fopen
    //by doing something similar to opensource.apple.com/source/Libc/Libc-167/stdio.subproj/fopen.c
    //TODO: is there a way to use a hashset or some variable arraylist of queues?
    if (!initialized){
        //TODO: how do I handle errors?
    }
    Queue *temp = head;
    while (temp->next != NULL){
        if (temp->q_name = q_name)
            return *temp;
        temp = temp->next;
    }
    //TODO: No match found. Return error.
}
int_least32_t q_publish(/*int_least32_t qd --changed to queue for now*/ Queue *q, int_least32_t *data, int_least32_t datalen)
{
    int kilo = 1024;
    if (sizeof(*data) > 128 * kilo){
        //TODO: throw an error
    }
    q->data = *data;
    q->datalen = datalen;
}
int_least32_t receiver(int_least32_t *userdata, int_least32_t *data, int_least32_t datalen)
{

}
int_least32_t q_subscribe(int_least32_t q, void (*receiver)(int_least32_t *userdata, int_least32_t *data, int_least32_t datalength), int_least32_t *userdata)
{
    //TODO: execute asynchronous callback with *receiver
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

int main()
{
    Stream s;
    memset(&s, 0, sizeof(s));
    q_create("someArg", "rr");
  //  create_buffer(&s, 10);

}
