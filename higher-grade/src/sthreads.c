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

// Function to remove (and return that) thread that is first in the ready list
thread_t *remove_from_ready_list() {
   if(!head){                 // Handle cases when the ready list is empty (so program dont crash)
      return NULL;
   }
   thread_t *next_thread = head->next;
   thread_t *current_head = head;
   head = next_thread;        // Make it so that the current head, now instead is what was the next thread (that the old thread pointed twords)
   
   while (true)
   {
      if(current_head->state == ready){
      //Free?????
      return current_head;
      }
      else{
         current_head->next = NULL;
         end->next = current_head;
      }
   }
   //return current_head;       // Return the thread that we removed from thread
}


/*******************************************************************************
                    Implementation of the Simple Threads API
********************************************************************************/

int init() { 

   return -1;  // On fail
   return 1;   // On success

}

tid_t spawn(void (*start)()) { return -1; }

void yield() {}

void done() {}

tid_t join(tid_t thread) { return -1; }
