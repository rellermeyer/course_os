#include <stdio.h>
#include <stdlib.h>

Node* volatile start=0;

void initialize_stack(){
    start=0;
    Node* n=(Node*)malloc(sizeof(Node));
    if (n==0){
        perror("node of stack");
        exit(-1);
    }
    //push the value on the stack
    n=(Node*)malloc(sizeof(Node));
    if(n==0){
        perror("second node of stack");
        exit(-1);
    }
    //push the second value
}
int node_func(Node* volatile * pointer, Node* old, Node* new);


void push_stack(Node* n){
    if(n == 0)return;
    if(node ==0) return;
    n->next =(Node *)node_func(&start,start,n);
    start=n;
}

Node* pop_stack(){
    Node* pointer=(Node *)node_func(&start,start,start);
    if (pointer !=0){
        start= pointer->next;
    }
    return pointer;
}

