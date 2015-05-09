#include "uthreads.h"
#include "klibc.h"

struct node {
    struct thread* data;
    struct node* next;
};

struct queue {
    size_t elements;
    struct node* head;
    struct node* tail;
};

size_t size(struct queue* q) {
    return q->elements;
}

struct thread* back(struct queue* q) {
    return q->tail->data;
}

struct thread* front(struct queue* q) {
    return q->head->data;
}

void push(struct queue* q, struct thread* data) {
    struct node* newNode = (struct node*) kmalloc(sizeof(struct node));
    newNode->data = data;
    newNode->next = 0;

    if (q->head == 0) {
        q->head = newNode;
    } else {
        q->tail->next = newNode;
    }
    q->tail = newNode;
    ++(q->elements);
}

void pop(struct queue* q) {
    struct node* remove = q->head;
    q->head = q->head->next;
    free(remove);
    --(q->elements);
}

struct thread {
    Boolean stackIsSet;
    void* arg;
    void* stack;
    void (*f)(void* arg);
    jmp_buf env;
};

static struct thread* current_thread;
static struct queue scheduled;
static jmp_buf env;

struct thread* thread_create(void (*f)(void *arg), void *arg) {
	struct thread* t = (struct thread*) malloc(sizeof(struct thread));
	t->stackIsSet = FALSE;
	t->arg = arg;
	t->stack = malloc(4096);
	t->f = f;
	return t;
}

void thread_add_runqueue(struct thread * t) {
	if (t != NULL && !(size(&scheduled) != 0 && back(&scheduled) == t)) {
		push(&scheduled, t);
	}
}

void thread_exit() {
	if (current_thread != NULL) {
		free(current_thread);
	}
	if (size(&scheduled) == 0) {
		longjmp(env, 5);
	} else {
		dispatch();
	}
}

void dispatch() {
	if (size(&scheduled) != 0) {
		current_thread = front(&scheduled);
		pop(&scheduled);
		if (current_thread->stackIsSet) {
			longjmp(current_thread->env, 1);
		} else {
			void* stack = (void*) ((size_t) (current_thread->stack) + 4096);
			__asm volatile("mov r14, %0": : "a" (stack));
			__asm volatile("mov r11, %0": : "a" (stack));
			current_thread->stackIsSet = TRUE;
			(*current_thread->f)(current_thread->arg);
		}
		thread_exit();
	}
}

void schedule(void) {
	thread_add_runqueue(current_thread);
}

void thread_yield(void) {
	if (size(&scheduled) != 0 && setjmp(current_thread->env) != 1) {
		schedule();
		dispatch();
	}
}

void thread_start_threading() {
	if (setjmp(env) != 5) {
		dispatch();
	}
}
