/**
 * bank.c
 *
 * The implementation of the simple bank API defined in bank.h.
 *
 * Author
 *   Karl Marklund <karl.marklund@it.uu.se>
 *
 */

#include "bank.h"  // The bank API

#include <stdbool.h>  // true, false
#include <stdio.h>    // perror()
#include <stdlib.h>   // exit(), EXIT_FAILURE
#include <unistd.h>   // usleep()

#define MIN_SLEEP 10000
#define MAX_RANDOM_SLEEP 10000

void RANDOM_SLEEP() { usleep(MIN_SLEEP + (rand() % MAX_RANDOM_SLEEP)); }

account_t *account_new(unsigned int balance) {
  account_t *account = malloc(sizeof(account_t));

  account->balance = balance;
  pthread_mutex_init(&account->lock, NULL); // Create the mutex lock for the account

  return account;
}

void account_destroy(account_t *account) {
  pthread_mutex_destroy(&account->lock); // Destory the mutex lock for the account
  free(account); // Delete the account
}

/**
 * A purposefully stupid way to add two numbers that makes data
 * races more likely.
 *
 * Do NOT add synchronization here.
 */
int add(int a, int b) {
  int result = a + b;
  RANDOM_SLEEP();
  return result;
}

/**
 * A purposefully stupid way to subtract two numbers that makes data
 * races more likely.
 *
 * Do NOT add synchronization here.
 */
int sub(int a, int b) {
  int result = a - b;
  RANDOM_SLEEP();
  return result;
}

int transfer(int amount, account_t *from, account_t *to) {
    pthread_mutex_lock(&from->lock); // Lock who we want to send from, so that the amount (that we check in if) doesnt get changed while we enter the if loop.
    if (from->balance >= amount) {
    from->balance = sub(from->balance, amount);

    /**
     * Don't remove this RANDOM_SLEEP. This is used to enforce a more
     * randomized interleaving of the threads.
     */
    RANDOM_SLEEP();
    pthread_mutex_lock(&to->lock); // Lock who we send to, so that the amount dont get changed somewhere else at the same time.

    to->balance = add(to->balance, amount);
    
    // Transfer is done, we can unlock the locks.
    pthread_mutex_unlock(&to->lock);
    pthread_mutex_unlock(&from->lock);

    return 0;
  } else {
    pthread_mutex_unlock(&from->lock); // Since we cant enter the if, we need to unlock here to not cause deadlock
    return -1;
  }
}
