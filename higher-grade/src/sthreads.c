/* On Mac OS (aka OS X) the ucontext.h functions are deprecated and requires the
   following define.
*/
#define _XOPEN_SOURCE 700

/* On Mac OS when compiling with gcc (clang) the -Wno-deprecated-declarations
   flag must also be used to suppress compiler warnings.
*/

#include <signal.h> /* SIGSTKSZ (default stack size), MINDIGSTKSZ (minimal
                         stack size) */
#include <stdio.h>  /* puts(), printf(), fprintf(), perror(), setvbuf(), _IOLBF,
                         stdout, stderr */
#include <stdlib.h> /* exit(), EXIT_SUCCESS, EXIT_FAILURE, malloc(), free() */
#include <ucontext.h> /* ucontext_t, getcontext(), makecontext(),
                         setcontext(), swapcontext() */
#include <stdbool.h>  /* true, false */
#include <unistd.h>

#include "sthreads.h"

/* Stack size for each context. */
#define STACK_SIZE SIGSTKSZ * 100

/*******************************************************************************
                             Global data structures

                Add data structures to manage the threads here.
********************************************************************************/

static thread_t *head = NULL;      // The thread that is in the firstmost position in the queue
static thread_t *end = NULL;       // 
static ucontext_t eliminator_ctx;
static ucontext_t scheduler_ctx;
static size_t list_size;
static size_t term_amount;
static int new_tid = 0;
static int sleep_time = 0;

// set up array with indexes being tid
static int tid_array_length = 32;
static thread_t **tid_arr;
//static tid_t next_tid = 0;                // Will be used when we need to create a new thread. So that all threads get different IDs

/*******************************************************************************
                             Print result

                      Add internal helper functions here.
********************************************************************************/
void print_contexts() {
   thread_t *start = head;
   int element = 0;
   while (start) {
      printf("element: %d, tid: %d\n", element, start->tid);
      start = start->next;
      element++;
   }
}

/*******************************************************************************
                             Auxiliary functions

                      Add internal helper functions here.
********************************************************************************/


// Function that adds a new thread (thread must already be created and non NULL) to the linked list of ready threads
void add_to_ready_list(thread_t *thread) {
   if (!thread) {             //Handle cases when the thread dont exists
      return; 
   }
   if (list_size > 0) {
      thread->next = NULL;
      end->next = thread;
      end = thread;
   } else {
      head = thread;
      end = thread;
   }
   list_size++;
}
void move_to_back() {
      end->next = head;
      end = head;
      head = head->next;
      end->next = NULL;
}
// Move thread to back
thread_t *shuffle_for_ready() {
   if(!head){                 // Handle cases when the ready list is empty (so program dont crash)
      return NULL;
   }
   
   while (true)
   {
      printf("Looking at %d\n", head->tid);
      sleep(sleep_time);
      if (head->state == waiting 
          && head->join_tid >= 0 
          && tid_arr[head->join_tid]->state == terminated) {
         head->state = ready;
      }
      if(head->state == ready){
         return head;
      }
      move_to_back();
   }
}

/*******************************************************************************
                             Contexts Management

                      Add internal helper functions here.
********************************************************************************/
/* Initialize a context.

   ctxt - context to initialize.

   next - successor context to activate when ctx returns. If NULL, the thread
          exits when ctx returns.
 */
void init_context(ucontext_t *ctx, ucontext_t *next) {
  /* Allocate memory to be used as the stack for the context. */
  void *stack = malloc(STACK_SIZE);

  if (stack == NULL) {
    perror("Allocating stack");
    exit(EXIT_FAILURE);
  }

  if (getcontext(ctx) < 0) {
    perror("getcontext");
    exit(EXIT_FAILURE);
  }

  /* Before invoking makecontext(ctx), the caller must allocate a new stack for
     this context and assign its address to ctx->uc_stack, and define a successor
     context and assigns address to ctx->uc_link.
  */

  ctx->uc_link           = next;
  ctx->uc_stack.ss_sp    = stack;
  ctx->uc_stack.ss_size  = STACK_SIZE;
  ctx->uc_stack.ss_flags = 0;
}
/* Initialize context ctx  with a call to function func with zero argument.
 */
void init_context0(ucontext_t *ctx, void (*func)(), ucontext_t *next) {
  init_context(ctx, next);
  makecontext(ctx, func, 0);
}

void scheduler() {
   while(true) {
      if(list_size <= term_amount) {
         printf("No more threads in action\n");
         exit(0);
      }
      printf("Looking for next ready\n");
      thread_t *result = shuffle_for_ready();
      result->state = running;
      printf("Found thread, %d\n", result->tid);
      if (swapcontext(&scheduler_ctx, &result->ctx) < 0 ) {
         perror("swapcontext into work thread failed\n");
         exit(EXIT_FAILURE);
      }
   }
}

void eliminator() {
   while(true) {
      head->state = terminated;
      term_amount++;
      printf("I killed thread %d!\n", head->tid);
      if (swapcontext(&eliminator_ctx, &scheduler_ctx) < 0) {
         perror("swapcontext eliminator failed\n");
         exit(EXIT_FAILURE);
      }
   }
}



/*******************************************************************************
                    Implementation of the Simple Threads API
********************************************************************************/

int init(void (*start)()) {
   tid_arr = calloc(sizeof(thread_t *), 32);
   init_context0(&scheduler_ctx, scheduler, NULL); 
   init_context0(&eliminator_ctx, eliminator, &scheduler_ctx); 
   // Get the main thread in
   spawn(start);
   if(setcontext(&scheduler_ctx) > 0) {
      perror("setcontext to scheduler failed\n");
      exit(EXIT_FAILURE);
   }
   return -1;   // On fail
}

tid_t spawn(void (*start)()) {
   thread_t *new_thread = malloc(sizeof(thread_t));
   if (new_thread == NULL) {
      return -1;
   }
   new_thread->tid = new_tid;
   printf("%p\n", tid_arr);
   printf("TID: %d\n", new_tid);
   // Adding to TID array
   if (new_tid >= tid_array_length) {
      tid_array_length = 2 * tid_array_length;
      tid_arr = realloc(tid_arr, tid_array_length);
   }
   printf("Made it here!\n");
   tid_arr[new_tid] = new_thread;
   printf("Made it here!\n");
   
   // Incrementing tid
   new_tid++;
   new_thread->state = ready;
   new_thread->join_tid = -1;
   init_context0(&new_thread->ctx, start, &eliminator_ctx);
   add_to_ready_list(new_thread);
   printf("Current list after add:\n");
   print_contexts();
   return new_tid;
}
void switch_to_scheduler() {
   move_to_back();
   if(swapcontext(&end->ctx, &scheduler_ctx) > 0) {
      perror("swapcontext eliminator failed\n");
      exit(EXIT_FAILURE);
   }
}

void yield() {
   head->state = ready;
   switch_to_scheduler();
}

void done() {
   setcontext(&eliminator_ctx);
}

tid_t join(tid_t thread) { 
   // check not out of bounds, TID thread exists.
   // State is not terminated and it is not the calling thread
   if (thread >= tid_array_length 
       && !tid_arr[thread]
       && tid_arr[thread]->state == terminated
       && thread == head->tid) {
      return -1; 
   }
   head->state = waiting;
   head->join_tid = thread;
   switch_to_scheduler();
   return thread;
}