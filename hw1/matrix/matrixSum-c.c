/* matrix summation using pthreads

   features: uses mutex to update global variables for sum, min and max. The sum is
             updated after each row and the min, max is updated as needed.
             Values are computed by Workers and printed to standard output upon all
             threads finishing.

   usage under Linux:
     gcc matrixSum-c.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t bag_mutex, sum_mutex, min_mutex, max_mutex;    /* Mutexes */

int numWorkers;           /* number of workers */ 
int next_row = 0;       /* My bag */

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

/* Struct to hold min and max */
typedef struct {
    int x, y;
    int value;
} Pos;

double start_time, end_time; /* start and end times */
int size;  /* assume size is multiple of numWorkers */
long sum;
Pos min, max;
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&bag_mutex, NULL);
  pthread_mutex_init(&sum_mutex, NULL);
  pthread_mutex_init(&max_mutex, NULL);
  pthread_mutex_init(&min_mutex, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  srand(7);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand()%99;
	  }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

  /* Init sum, min, max */
  sum = 0;
  min.value = matrix[0][0];
  max.value = matrix[0][0];
  min.x = min.y = max.x = max.y = 0;    /* Actually initialized to 0 as globals */

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);

  /* Wait for all threads */
  for (l = 0; l < numWorkers; l++)
      pthread_join(workerid[l], NULL);

  /* get end time */
  end_time = read_timer();

  /* print results */
  printf("The total is %ld, min is %d at [%d,%d], max is %d at [%d,%d]\n", sum,
          min.value, min.y, min.x, max.value, max.y, max.x);
  printf("The execution time is %g sec\n", end_time - start_time);

  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;
  long total;
  int row, j;

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  while (1) {
      /* Fetch new task */
      pthread_mutex_lock(&bag_mutex);
      row = next_row++;
      pthread_mutex_unlock(&bag_mutex);

      if (row >= size) /* No more task - exit */
          pthread_exit(NULL);

      /* sum values in my strip */
      total = 0;
      for (j = 0; j < size; j++) {
        total += matrix[row][j];
        
        /* Update min and max */
        if (min.value > matrix[row][j]) {
            pthread_mutex_lock(&min_mutex);
            if (min.value > matrix[row][j]) {
                min.value = matrix[row][j];
                min.x = j;
                min.y = row;
            }
            pthread_mutex_unlock(&min_mutex);
        } else if (max.value < matrix[row][j]) {
            pthread_mutex_lock(&max_mutex);
            if (max.value < matrix[row][j]) {
                max.value = matrix[row][j];
                max.x = j;
                max.y = row;
            }
            pthread_mutex_unlock(&max_mutex);
        }
      }

      /* Update sum */
      pthread_mutex_lock(&sum_mutex);
      sum += total;
      pthread_mutex_unlock(&sum_mutex);
  }

  /* Exit */
  pthread_exit(NULL);
}
