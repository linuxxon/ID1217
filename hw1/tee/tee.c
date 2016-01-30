/* tee.c - multi threaded implementation of the UNIX tee program
 *
 * Homework 1 - Problem 4: The Unix tee command (20 points)
 *
 * The program reads standard input and writes it both to standard output and
 * to the filename specified by the first argument
 *
 * Semantics:
 *  Shtuff
 *
 * Usage:
 *  <generate output> | tee filename | <output file>
 *
 * Compilation:
 *  gcc tee.c -lpthread
 *
 * For course ID1217 at KTH
 *
 * Author: Rasmus Linusson
 * Last modified: 29/01-2016
 */ 

/* Fetch all the right functions */
#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER 100

/* Thread task functions */
void* in_stream(void* arg);
void* out_stream(void* arg);
void* out_file(void* arg);

/* Circular buffer to hold string from standard input to be handled */
char* buffer[MAX_BUFFER];

/* Buffer bound variables */
int buff_size   = 0;    /* Number of elements currently held in buffer */
int buff_write  = 0;    /* Next slot to write to */
int buff_read   = 0;    /* Last slot not ready to be overwritten */
int buff_stream, buff_file; /* Reading positions for the two threads */

/* Locks and condition variables for accessing, and moving buffer bound */

/* buff_stream/file need no mutexes as even if there is miss information the
 * counter will catch up after the next read so who cares, buffer being full is
 * not a problem here either as the readers can continue to read  and then
 * update the counter */
pthread_mutex_t buff_size_mutex, buff_read_mutex;
pthread_cond_t buff_full, buff_empty;

int main(int argc, char* argv[])
{
    char *filename = NULL;  /* Name of file to write to */

    /* Read command line argument */
    if (argc > 1)
        filename = argv[1];

    /* Start a thread to read from standard input */
    pthread_create(NULL, NULL, in_stream, NULL);

    /* Start a thread to write to standard output */
    pthread_create(NULL, NULL, out_stream, NULL);

    /* Let main thread do output to file */
    out_file((void*) filename);
    pthread_exit(NULL);
}

/* Read standard input and store strings in buffer */
void* in_stream(void * arg)
{
}

/* Print buffer to standard output */
void* out_stream(void * arg)
{
}

/* Write buffer to file */
void * out_file(void * arg)
{
}
