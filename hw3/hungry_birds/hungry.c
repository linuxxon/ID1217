/* hungry.c - The Hungry Birds Problem (one producer - multiple consumers) (20 points)
 *
 * Homework 3 - Problem 1: The Hungry Birds Problem (20 points)
 *
 * The question of fairness (Same as in README):
 *  A part of the assignment is to make a comment as to the fairness of the
 *  algorithm. This implementation is **NOT**  fair with regard to the birds,
 *  this wholly because of the implementation of the semaphores. 
 *  The semaphores are kind of similar to first come, first serve and holds no
 *  regard as to the invocation order of 'sem_wait'. 
 *  With this behavior there is no guarantee that every baby bird will get to
 *  eat as there is a risk of the other baby birds always getting through the
 *  semaphore every time there are worms available.
 *
 * Semantics:
 *  See full description in README
 *
 * Usage:
 *  ./hungry
 *
 * Compilation:
 *  gcc hungry.c -pthread
 *
 * For course ID1217 at KTH
 *
 * Author: Rasmus Linusson
 * Last modified: 19/02-2016
 */ 

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

#define MAX_BIRDS 10
#define DEFAULT_BIRDS 3

#define MAX_CAPACITY 100
#define DEFAULT_CAPACITY 10

#define MAX_WORM_ADDITION MAX_CAPACITY
#define DEFAULT_WORM_ADDITION DEFAULT_BIRDS*2

/* Define baby bird sleep interval in microseconds */
#define MAX_SLEEP 1000*1000
#define MIN_SLEEP  500*1000

/* Set done to non-zero to kill threads */
int done = 0;

/* Simulation settings */
int worm_addition;
int dish_capacity;

/* Semaphores */
sem_t check_dish;
sem_t get_worm;
sem_t go_fetch;

/* Thread functions */
void* baby_bird(void* arg);
void* parent_bird(void* arg);

/* Entry point
 *
 * Takes user arguments, inits variable and spawn threads
 *
 * Takes three optional arguments:
 *  First: int - number of baby birds in the nest
 *  Second: int - maximum number of worms kept in the dish
 *  Third: int - maximum number of worms parent bird might find
 */
int main(int argc, char* argv[])
{
    long i;
    char x[2];

    /* Set defaults */
    int num_birds = DEFAULT_BIRDS;
    dish_capacity = DEFAULT_CAPACITY;
    worm_addition = DEFAULT_WORM_ADDITION;

    pthread_t tid;          /* Throwaway id */

    /* Get arguments */
    if (argc > 1)
        num_birds = atoi(argv[1]) > MAX_BIRDS ? MAX_BIRDS : atoi(argv[1]);
    if (argc > 2)
        dish_capacity = atoi(argv[2]) > MAX_CAPACITY ? MAX_CAPACITY : atoi(argv[2]);
    if (argc > 3)
        worm_addition = atoi(argv[3]) > MAX_WORM_ADDITION ? MAX_WORM_ADDITION : atoi(argv[3]);

    /* Init semaphores */
    sem_init(&get_worm, 0, 0);          /* Empty dish */
    sem_init(&check_dish, 0, 1);        /* Mutex */
    sem_init(&go_fetch, 0, 0);          /* Noone has complained yet */

    /* Print welcome message */
    printf("Welcome to The Hungry Birds Problem!\nThere are %d baby birds and 1"
            "parent bird\nThe dish can hold %d worms and the parent fetches 1 - %d"
            "new worms every time it is woken.\nLet's start!\n",
            num_birds, dish_capacity, worm_addition);

    /* Spawn children */
    for (i = 0; i < num_birds; i++)
        pthread_create(&tid, NULL, baby_bird, (void*)i);

    /* Spawn parent */
    pthread_create(&tid, NULL, parent_bird, NULL);
    
    /* Main thread is waiting for anything to exit */
    fgets(x, 2, stdin);

    /* Tell all threads to stop looping */
    done++;
    sem_post(&go_fetch);
    for (i = 0; i <= num_birds; i++)
        sem_post(&get_worm);

    pthread_exit(0);
}

/* baby_bird - function describing behaviour of baby birds
 *
 * Arguments
 *  void* arg - long baby bird id cast to a void pointer
 */
void* baby_bird(void* arg)
{
    pthread_detach(pthread_self());

    int id = (int)(long)arg;

    /* Local variables to keep result from sem_getvalue */
    int num_worms;
    int told_parent;

    while (!done)
    {
        sem_wait(&check_dish);  /* Lock when checking size */
        printf("Baby bird %d is hungry\n", id);

        sem_getvalue(&get_worm, &num_worms);

        if (num_worms > 0) {        /* Worms available - eat them */
            sem_wait(&get_worm);    /* Garanteed to work as locked by check_dish */
            sem_post(&check_dish);  /* Release lock */
        }
        else {                      /* No worms available */
            /* Only one baby bird should chirp loudly */
            sem_getvalue(&go_fetch, &told_parent);
            if (!told_parent) {
                printf("Baby bird %d notised there are no worms, *CHIIIIIIRP!!!*\n", id);
                sem_post(&go_fetch);  /* Notify parent bird to fetch more worms */
            }

            sem_post(&check_dish);  /* Release lock */
            sem_wait(&get_worm);    /* Wait on parent bird to relase worms into dish */
        }

        printf("Baby bird %d just ate\n", id);

        /* Sleep random time */
        usleep(rand() % MAX_SLEEP + MIN_SLEEP + 1);
    }

    printf("Bird %d leaving the nest\n", id);

    pthread_exit(0);
}

/* parent_bird - function defining behavior of parent bird */
void* parent_bird(void* arg)
{
    pthread_detach(pthread_self());

    int i;
    int worms_fetched;

    while (!done)
    {
        /* Wait for baby birds to ask for more food */
        printf("Parent bird is sleepy...\n");
        sem_wait(&go_fetch);

        printf("Parent bird is awaken by a hungry baby bird\n");
        sem_post(&go_fetch);    /* Indicator that parent is awake */

        /* Fly off */
        /* Random time delay? */

        /* Add worms to dish */
        worms_fetched = rand() % worm_addition + 1;
        printf("Parent bird found %d worms for the baby birds\n", worms_fetched);
        for (i = 0; i < worms_fetched; i++)
            sem_post(&get_worm);

        sem_wait(&go_fetch);    /* Parent is back, remove indicator */
    }

    printf("Parent bird died :'(\n");

    pthread_exit(0);
}
