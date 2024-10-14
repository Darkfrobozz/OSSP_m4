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

#include "sthreads.h"

/* Stack size for each context. */
#define STACK_SIZE SIGSTKSZ * 100

/*******************************************************************************
                             Global data structures

                Add data structures to manage the threads here.
********************************************************************************/

static thread_t *head = NULL;      // The thread that is in the firstmost position in the queue
static thread_t *end = NULL;       // 
static thread_t *eliminator = NULL;
static thread_t *scheduler = NULL;
//static tid_t next_tid = 0;                // Will be used when we need to create a new thread. So that all threads get different IDs

/*******************************************************************************
                             Auxiliary functions

                      Add internal helper functions here.
********************************************************************************/


// Function that adds a new thread (thread must already be created and non NULL) to the linked list of ready threads
void add_to_ready_list(thread_t *thread) {
   if (!thread) {             //Handle cases when the thread dont exists
      return; 
   }
   thread->next = NULL;
   end->next = thread;
}

// Move thread to back
thread_t *shuffle_for_ready() {
   if(!head){                 // Handle cases when the ready list is empty (so program dont crash)
      return NULL;
   }
   
   while (true)
   {
      printf("Looking at %d", end->tid);
      sleep(2);
      end->next = head;
      end = head;
      head = head->next;
      end->next = NULL;
      if(end->state == ready){
         return end;
      }
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
   printf("Looking for next ready");

}

void eliminator() {

}


/*******************************************************************************
                    Implementation of the Simple Threads API
********************************************************************************/

int init() {
   
   return 1;   // On success
}

tid_t spawn(void (*start)()) { return -1; }

void yield() {}

void done() {}

tid_t join(tid_t thread) { return -1; }
