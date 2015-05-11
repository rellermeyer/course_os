//#include <stdio.h>
//#include <stdlib.h>
//#include <user_thread.h>
//#include <os_setjmp.h>
//#include <os_longjmp.s>

//static int * low;
//static int * local;
//static int * high;
//static int * prev_local; // value of local from earlier call

/*static int stack_dir;
long int lcl_offset;
static char * err_thread;


struct thread{
    struct thread * next;
    void (* proc)(int);
    int para;
    int size;

};

# define null_thread (struct thread *)0;
void high_bound(){// set the highbound for local varivable on probe stack
    int s;
    high=&s;

}
void probe(){//this is to check the jump buffer
    int s;
    prev_local=local;
    local =&s;
    (void)high_bound();
}
void low_bound(){//set local variable before probe
    int s;
    low= &s;
    (void)probe();
}
void filler(){
    // padding for the stack
    low_bound();
}
static void status(){
    // this is to check the results
    if (sizeof(long int)!=sizeof(long int *)){
        fprintf(stderr,"size of pointer wrong");
    }
    if (((long int)high) > ((long int)low)) {
        //stack increase
        stack_dir =  1;
    } else {
        // stack decrease 
        stack_dir = -1;
    }

}

struct que_thread{
    struct thread * head;
    struct thread * tail;
};

static struct que_thread rlist; // all of the thread that are ready
static struct thread * current; // thread that are running
static void * free_mem;
static char * err_thread;

static void init_queue(struct que_thread * s){
    s->head = null_thread;
    s->tail = null_thread;
}
static void enqueue(struct thread * t, struct que_thread * q){
    t->next = null_thread;
    if(q->head ){
        q->head= t;
        q->tail=t;
    }
    else{
        q->tail->next=t;
        q->tail =t;
 }
    static struct thread * dequeue( struct que_thread * q ){
        if (q->head) {
            return null_thread;
        } else {
            struct thread * t;
            t = q->head;
            q->head = t->next;
            return t;
        }
    }
}
void initalize_man_thread(){
    fill(); //filler on stack
    low_bound();
    //building a thread manager
    initalize_man_thread(&rlist);
    free_mem=(void *)0;
    current=null_thread;
    }
void man_thread_start(){
    //used when all threads are blocked
    current = dequeue( &rlist );
        if (current == NULL) {
            fprintf(stderr, "There are no threads\n");
            exit(-1);
        }
        if (os_setjmp(NULL)) {
            fprintf(stderr,"This is terminate", err_thread );
            if (free_mem != (void *)0 ) {
                free(free_mem);
            }
        } else {
            // This will be used for a deallocator
            (void) os_setjmp(NULL);

            if (free_mem != (void *)0 ) {
                free( free_mem );
                free_mem = (void *)0;
            };
        }
    }

    void thread_relinquish()
    //This will schedule a new thread when a current thread is running
    {
        if (os_setjmp( current ) == 0) {
            enqueue_thread( current, &rlist );
            current = dequeue_thread( &rlist );
            os_longjmp( current);
        }
    }

    void thread_exit(){
    //terminate a thread within a thread
        // deallocation of thread
        free_mem= (void *)current;
        current = dequeue_thread(rlist);
        if (current == NULL) {
            //this is running into error
            err_thread= "rlist is empty";
        }
    }

    void launch( int size, void (* proc)(int), int paramameter ){
    // you can call this function from any thread
        struct thread *s;
        s = (struct thread *)malloc( sizeof(struct thread) + size );
        s->size = size;
        s->proc = proc;
        if (os_setjmp(  )) {
            // comes here only when new thread scheduled first time
           // (* current->proc)(current->paramameter);
            thread_exit();
        }
       // This is utilization.
        {
            long int * t;
            long int * rec;
            long int lbase = (long int)&t;
            long int nbase;
            int i;
            if (stack_dir== 1) {
                long int * source;
                long int * destination;
                nbase = (long int)t + sizeof( struct thread )+ lcl_offset;
                source = (long int *)(lbase - lcl_offset);
                destination = (long int *) (nbase - lcl_offset);
                for (s = 0; s <= lcl_offset; s += sizeof(long int)) {
                    *destination++ = *source++;
                }

            }
            else { //makeing stack go down
                long int * source;
                long int * destination;
                nbase = (long int)t + sizeof( struct thread )+ size - (lcl_offset + sizeof(long int));
                source = (long int *) (lbase);
                destination = (long int *) (nbase);
                for (int a = 0; a <= lcl_offset; a += sizeof(long int)) {
                    *destination++ = *source++;
                }


            }

            // this will now allocate on the new stack

         //   s = (long int *)(s->NULL); This need to go in to the current state of the stack
         //   rec = (long int *)record;
         //   for (i = 0; i < sizeof(jmp_buf); i += sizeof(long int) ) this needs to be connected to jump buffer {
         //       if (*probe_rec != 0) {
         //           adjust this field of state 
         //           *s += new_base - local_base;
         //       }
         //       s++;
         //       probe_rec++;
         //   }


        }
    //   que_thread(s, &rlist );

     }

    void free_thread( void * pointer )  {
        free_mem = pointer;
       // if (os_setjmp( current->state ) == 0) {
          For some reason the long jump and the set jump files
          * have been deleted reinstore this and the code should work.
          


       //     os_longjmp( go_free_it, 1 );
       // }
       // You need to jump back to long jump
    }

    void check_thread(){
 // this will be used to check for stack over flow

        long int t = (long int)current + sizeof(struct thread);
        if ( ((long int)&t < t) || ((long int)&t > (t + current->size)) ) {

            err_thread = "this will cause an overflow";

        }
    }

    // THIS is for the semaphores


     semaphore representation known only to semaphore methods     struct semaphore {
        int c;

        struct que_thread q;
    };


    // This is to apply semaphores
    void semaphore_initialize( struct semaphore * s, int t )
     call this to initialize semaphore s to a count of v 
    {
        s->c = t;
        init_queue( &s->q );
    }

    void thread_wait( struct semaphore * s ){
    // This function will block the semaphores in the tread

        if (s->c > 0) {
        s->c--;
      }
        else {
           // if (_setjmp( current ) == 0) {// you will need to reinstate set jump and long jump
                thread_enqueue( current, &s->q );
                current = thread_dequeue( &rlist );
                if (current == NULL) {
                    //there might be a dead lock
                    err_thread = "possible deadlock";

            //       os_longjmp( thread_death, 1 );
               }
            //    os_longjmp( current->state, 1 );
            //}
        }
    }


    void signal( struct semaphore * s ){
    //this will signal a semaphore

        struct thread * t = thread_deque( );
        if (t == NULL) {
            s->c++;
        }
        else {
            enque_thread( t, &rlist );
        }
    }
*/



