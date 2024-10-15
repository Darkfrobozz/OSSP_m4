#include <stdlib.h>   // exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <stdio.h>    // printf(), fprintf(), stdout, stderr, perror(), _IOLBF
#include <stdbool.h>  // true, false
#include <limits.h>   // INT_MAX

#include "sthreads.h" // init(), spawn(), yield(), done()
#include <unistd.h>

/*******************************************************************************
                   Functions to be used together with spawn()

    You may add your own functions or change these functions to your liking.
********************************************************************************/

/* Prints the sequence 0, 1, 2, .... INT_MAX over and over again.
 */
void numbers() {
  int n = 0;
  while (true) {
    printf(" n = %d\n", n);
    n = (n + 1) % (INT_MAX);
    if (n > 3) done();
    sleep(1);
}
}

/* Prints the sequence a, b, c, ..., z over and over again.
 */
void letters() {
  char c = 'a';

  while (true) {
      printf(" c = %c\n", c);
      if (c == 'f') done();
      yield();
      c = (c == 'z') ? 'a' : c + 1;
    }
}

/* Calculates the nth Fibonacci number using recursion.
 */
int fib(int n) {
  switch (n) {
  case 0:
    return 0;
  case 1:
    return 1;
  default:
    return fib(n-1) + fib(n-2);
  }
}

/* Print the Fibonacci number sequence over and over again.

   https://en.wikipedia.org/wiki/Fibonacci_number

   This is deliberately an unnecessary slow and CPU intensive
   implementation where each number in the sequence is calculated recursively
   from scratch.
*/

void fibonacci_slow() {
  int n = 0;
  int f;
  while (true) {
    f = fib(n);
    if (f < 0) {
      // Restart on overflow.
      n = 0;
    }
    printf(" fib(%02d) = %d\n", n, fib(n));
    n = (n + 1) % INT_MAX;
  }
}

/* Print the Fibonacci number sequence over and over again.

   https://en.wikipedia.org/wiki/Fibonacci_number

   This implementation is much faster than fibonacci().
*/
void fibonacci_fast() {
  int a = 0;
  int b = 1;
  int n = 0;
  int next = a + b;

  while(true) {
    printf(" fib(%02d) = %d\n", n, a);
    next = a + b;
    a = b;
    b = next;
    n++;
    if (a < 0) {
      done();
      // Restart on overflow.
      //a = 0;
      //b = 1;
      //n = 0;
    }
  }
}

/* Prints the sequence of magic constants over and over again.

   https://en.wikipedia.org/wiki/Magic_square
*/
void magic_numbers() {
  int n = 3;
  int m;
  while (true) {
    m = (n*(n*n+1)/2);
    if (m > 0) {
      printf(" magic(%d) = %d\n", n, m);
      n = (n+1) % INT_MAX;
    } else {
      // Start over when m overflows.
      n = 3;
    }
    yield();
  }
}

/*******************************************************************************
                                     main()

            Here you should add code to test the Simple Threads API.
********************************************************************************/

//Test to try out join to ensure that our main thread waits
void test_basic_thread_execution() {
  puts("\n==== Show that join works Test ====\n");
  int tid1 = spawn(numbers);
  int tid2 = spawn(letters);
  join(tid1);
  join(tid2);
  puts("\n==== Test Completed (after threads are done) ====\n");
}

// Test for just running one thread (besides main)
void test_basic_thread(){
  puts("\n==== Very Basic Test ====\n");
  spawn(numbers);
  puts("\n==== Test function Completed (before numbers are done) ====\n");
}

//Shows numbers getting preemptive
void test_preemptive_show(){
  puts("\n==== Preemptive between 2 threads Test ====\n");
  spawn(numbers);
  spawn(fibonacci_fast);
  puts("\n==== Test function Completed (before threads are done) ====\n");
}

//Shows when fib gets large (and slow) that it gets preemptive a lot of time while trying to solve just one number
void test_preemptive_show_many(){
  puts("\n==== Clear Preemptive Test ====\n");
  spawn(fibonacci_slow);
  puts("\n==== Test function Completed (before threads are done) ====\n");
}

// Ensures that join returns immediately
void test_multiple_joins() {
  puts("\n==== Multiple Joins Test ====\n");
  int tid = spawn(numbers);
  int join1 = join(tid);
  int join2 = join(tid);  // Should return immediately since the thread is already terminated

  if (join1 == tid && join2 == tid) {
    printf("Both joins returned the correct thread ID: %d\n", tid);
  } else {
    printf("Join failed. Expected %d, got %d and %d\n", tid, join1, join2);
  }
  puts("\n==== Test Completed (after numbers finished)====\n");
}


int main(){
  puts("\n==== Test program for the Simple Threads API ====\n");
  //init(test_preemptive_show_clearly);
  //init(test_preemptive_show);
  //init(test_basic_thread_execution); // Initialization
  //init(test_basic_thread);
  //init(test_multiple_joins);
  perror("Wrong place");    // We should not be able to reach this place if we have used "init"
}

