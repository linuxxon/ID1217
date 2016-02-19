/* bear.c - The Bear and Honeybees Problem (20 points)
 *
 * Homework 3 - Problem 2: The Bear and Honeybees Problem (20 points)
 *
 * Semantics:
 *  See full description in README
 *
 * The question of fairness (Same as in README):
 *  A part of the assignment is to make a comment as to the fairness of the
 *  algorithm, with regard to the honeybees. This implementation is **NOT**
 *  fair with regard to the bees, this wholly because of the implementation of
 *  the semaphores. The semaphores are kind of similar to first come, first
 *  serve and holds no regard as to the invocation order of 'sem_wait'. 
 *  With this behaviour one unlucky bee may never get to deposit it's portion
 *  of honey into the pot because of never getting the semaphore.
 *
 * Usage:
 *  ./bear.c
 *
 * Compilation:
 *  gcc bear.c -pthread
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

#define MAX_BEES 10
#define DEFAULT_BEES 3

#define MAX_CAPACITY 100
#define DEFAULT_CAPACITY 10

/* Define range of time taken to gather one portion of honey in microseconds */
#define MAX_SLEEP 1000*1000
#define MIN_SLEEP  500*1000

/* Set done to non-zero to kill threads */
int done = 0;

int pot_capacity;           /* Size of the honeypot */
int pot_portions;           /* Number of portions contained in pot */

int num_bees;               /* Number of honeybees */

/* Semaphores */
sem_t pot_lock;
sem_t pot_full;
sem_t pot_empty;

/* Threaded functions */
void* bee(void* arg);
void* bear(void* arg);

/* Entry point of program, inits variables, spawns threads and waits on signal
 * from user to exit
 * 
 * Takes two optional arguments:
 *  First argument: int - Number of honeybees
 *  Second argument: int - Size of the honepot
 */
int main(int argc, char* argv[])
{
    long i;
    char x[2];  /* To provide a nice exit with fgets */

    /* Set defaults */
    num_bees = DEFAULT_BEES;
    pot_capacity = DEFAULT_CAPACITY;

    pthread_t tid;          /* Throwaway id */

    /* Get arguments */
    if (argc > 1)
        num_bees = atoi(argv[1]) > MAX_BEES ? MAX_BEES : atoi(argv[1]);
    if (argc > 2)
        pot_capacity = atoi(argv[2]) > MAX_CAPACITY ? MAX_CAPACITY : atoi(argv[2]);

    /* Init semaphores */
    sem_init(&pot_lock, 0, 1);      /* Mutex */
    sem_init(&pot_full, 0, 0);      /* Used to wake bear */
    sem_init(&pot_empty, 0, num_bees);  /* Used to wake bees */

    pot_portions = 0;       /* Init with empty honepot */

    /* Print welcome message */
    printf("Welcome to The Bear and Honeybees Problem!\nThere are %d honeybees and 1"
            "bear\nThe honeypot can hold %d portion and a bee deposits one portion"
            "each time.\nWhen the pot is full the bear is woken. Let's start!\n\n",
            num_bees, pot_capacity);

    /* Spawn children */
    for (i = 0; i < num_bees; i++)
        pthread_create(&tid, NULL, bee, (void*)i);

    /* Spawn bear */
    pthread_create(&tid, NULL, bear, NULL);
    
    /* Main thread is waiting for anything to exit */
    fgets(x, 2, stdin);

    /* Tell all threads to stop looping */
    done++;
    sem_post(&pot_full);    /* Wake a sleeping bear */
    for (i = 0; i < num_bees; i++)
        sem_post(&pot_empty);

    pthread_exit(0);
}

/* bee - function defining behavior of a honeybee
 *
 * Arguments:
 *  void* arg - A long cast to a void pointer representing the ID of the  honeybee
 */
void* bee(void* arg)
{
    pthread_detach(pthread_self());

    int id = (int)(long)arg;

    while (!done)
    {
        /* Fly fetch some honey */
        usleep(rand() % MAX_SLEEP + MIN_SLEEP + 1);

        /* Got one unit of honey! */

        printf("[Bee %d] waiting to deposit honey\n", id);
        sem_wait(&pot_lock);    /* Bear locks during eating time */
        if (pot_portions < pot_capacity) {
            pot_portions++;

            printf("[Bee %d] deposited one unit of honey\n", id);

            if (pot_portions == pot_capacity)
                sem_post(&pot_full);            /* Wake bear */
            sem_post(&pot_lock);
        }
        else {
            /* Pot is full, wait for it to be empty */
            sem_post(&pot_lock);
            sem_wait(&pot_empty);
        }
    }

    printf("[Bee %d] dropped dead...\n", id);

    pthread_exit(0);
}

/* bear - function defining behavior of the bear
 *
 * No arguments
 */
void* bear(void* arg)
{
    pthread_detach(pthread_self());

    int i;

    while (!done)
    {
        printf("*Bear is sleeping\n");
        sem_wait(&pot_full);        /* Wait until the bees have filled to pot */

        sem_wait(&pot_lock);        /* Make sure the bees aren't home */
        pot_portions = 0;           /* Yummy! */

        /* Wake all bees */
        for (i = 0; i < num_bees; i++)
            sem_post(&pot_empty);

        sem_post(&pot_lock);        /* Go away and let the bees gather more */

        printf("*Bear at it all!\n");
    }

    printf("*Bear got sick of honey and left\n");

    pthread_exit(0);
}
