/*
 * Author: Maximilian Kerp
 * Group: 5
 * Date: 05.06.14
 * Purpose: The purpose of this exercise is to get to know
 * the concept and proper usage of mutexes. The problem to solve
 * is the following: You have a set of bank accounts wich store
 * the amount of money you own. Your program should be able to
 * transfer money from one account to an other in a thread-safe
 * manner. So multiple threads should be able to work on the given
 * set of accounts, without corrupting the data. In order to asure
 * that two transfers on the same account won't mess up the data
 * you have to lock the access to one account with a mutex.
 * The following program does exactly that.
 */
#include <stdio.h>      // printf(), etc..
#include <stdlib.h>     // calloc()
#include <pthread.h>    // pthread_create(), *_mutex_lock(), *_mutex_unlock()

#define NUM_ACCS 6
#define NUM_THREADS 3

// We create two arrays, one for the account balances and
  // one for the mutexes. Every account has his one mutex
  // this we allocate them on the heap there are globally
  // visible.
 pthread_mutex_t account_mutexes[NUM_ACCS];
 volatile float account_balances[NUM_ACCS];

//===========================================================================
// the function wich transfers the money, we lock/unlock the mutex here
int
process_transfer (int from_acct, int to_acct, float amount)
{

  printf("  ...transfering %.2f€ from Account#%d to Account#%d\n", amount, from_acct,
      to_acct);

  pthread_mutex_lock(&account_mutexes[from_acct]);
  account_balances[from_acct] -= amount;
  pthread_mutex_unlock(&account_mutexes[from_acct]);

  pthread_mutex_lock(&account_mutexes[to_acct]);
  account_balances[from_acct] += amount;
  pthread_mutex_unlock(&account_mutexes[to_acct]);

  return 0;
}


//===========================================================================
// the function wich is executed by each thread
void *
thread_fun ( void *args)
{
  int *transfer_args = args;

  // generate random amount of money to be transferred
  float amount = ((float) rand() / (float) (RAND_MAX)) * 10000.0;

  process_transfer(transfer_args[0], transfer_args[1], amount);

  return NULL;
}

//================================================================================
// MAIN
int main ( int argc, char **argv)
{
  int i;
  // seeding rand so we allways get diffrent balances
  srand( (unsigned int) time(NULL) );



  for( i = 0; i < NUM_ACCS; i++) {
    pthread_mutex_init( &account_mutexes[i], NULL);
    account_balances[i] = ((float) rand() / (float) (RAND_MAX)) * 100000.0;

    printf("Account #%d: %.2f€.\n", i, account_balances[i]);
  }

  int transfer_args[3][2] = { {0,1}, {1,2}, {4,5} };

  // I will create 3 threads, 2 of them will access the same data.

  pthread_t threads[NUM_THREADS];
  for ( i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, thread_fun, &transfer_args[i]);
  }


  for ( i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  for (i = 0; i < NUM_ACCS; i++) {
    printf("Account #%d: %.2f€.\n", i, account_balances[i]);
  }

  return 0;
}
