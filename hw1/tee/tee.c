/* tee.c - multi threaded implementation of the UNIX tee program
 *
 * Homework 1 - Problem 4: The Unix tee command (20 points)
 *
 * The program reads standard input and writes it both to standard output and
 * to the filename specified by the first argument
 *
 * Semantics:
 *  See full description in README
 *
 * Usage:
 *  tee [filename] [ | program ] 
 *
 * Compilation:
 *  gcc tee.c -lpthread
 *
 * For course ID1217 at KTH
 *
 * Author: Rasmus Linusson
 * Last modified: 30/01-2016
 */ 

/* Fetch all the right functions */
#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER 100
#define MAX_LENGTH 200

/* Thread task functions */
void* in_stream(void*);
void* out_worker(void*);

/* Circular buffer to hold string from standard input to be handled
 * If only two readers 'read' can be used as boolean {0,1}.
 *
 * If more readers are necessary read can be used with a mask
 * letting read=2^(n-1)-1 signify that all readers have read the slot
 * */
typedef struct {
    char* str;
    unsigned read;
} buff_slot;

buff_slot buffer[MAX_BUFFER];

/* Buffer bound variables */
int buff_size = 0;    /* Number of elements currently held in buffer */
int buff_write = 0; /* Next slot to write to */

/* Indicator set by in_stream to indicate end of stream */
int done = 0;

/* Locks and condition variables for accessing, and moving buffer bound */

/* buff_stream/file need no mutexes as even if there is miss information the
 * counter will catch up after the next read so who cares, buffer being full is
 * not a problem here either as the readers can continue to read  and then
 * update the counter */
pthread_mutex_t buff_size_mutex, buff_read_mutex, buff_write_mutex;
pthread_cond_t buff_full, buff_empty;

/* Print standard input to filename provided and to standard output */
int main(int argc, char* argv[])
{
    int i;
    char *filename = NULL;  /* Name of file to write to */
    FILE* fp;
    pthread_t tid;

    /* Read command line argument */
    if (argc > 1)
    {
        filename = argv[1];
        fp = fopen(filename, "w");

        if (fp == NULL)
            fprintf(stderr,
                    "Can't open file %s for reading. Still redirecting to stdout",
                    filename);
    }

    /* Init buffer */
    for (i = 0; i < MAX_BUFFER; i++)
        buffer[i].str = (char *) malloc(sizeof(char)*MAX_LENGTH);

    /* Start a thread to read from standard input */
    pthread_create(&tid, NULL, in_stream, NULL);

    /* Start a thread to write to file */
    if (fp != NULL)
        pthread_create(&tid, NULL, out_worker, (void *) fp);

    /* Let main thread do output to standard output */
    out_worker((void *)stdout);

    /* Free buffer 
    for (i = 0; i < MAX_BUFFER; i++)
        free(buffer[i].str);

    pthread_exit(NULL);
    */
}

/* Read standard input and store strings in buffer */
void* in_stream(void * arg)
{
    /* Detach thread */
    pthread_detach(pthread_self());

    /* Do work */
    while (1)
    {
        if (buff_size < MAX_BUFFER) /* There's room left */
        {
            if (fgets(buffer[buff_write].str, MAX_LENGTH, stdin) != NULL)
            {
                /* Init read flag */
                buffer[buff_write].read = 0;

                /* Update for everyone else to see */
                pthread_mutex_lock(&buff_size_mutex);
                buff_size++;                        /* Update buffer size */
#ifdef DEBUG
                printf("Instream at slot %d buff_size %d\n", buff_write, buff_size);
#endif
                pthread_mutex_unlock(&buff_size_mutex);

                /* Update write slot */
                pthread_mutex_lock(&buff_write_mutex);
                buff_write = (buff_write + 1) % MAX_BUFFER;
                pthread_cond_signal(&buff_empty);   /* If someone was waiting, set them free */
                pthread_mutex_unlock(&buff_write_mutex);
            }
            if (feof(stdin))
            {
#ifdef DEBUG
                printf("In stream is ended\n");
#endif
                /* Signal all waiting to wake up and exit */
                pthread_mutex_lock(&buff_size_mutex);
                done = 1;
                pthread_cond_broadcast(&buff_empty);
                pthread_mutex_unlock(&buff_size_mutex);
                pthread_exit(NULL);
            }
        }
        else    /* Buffer is full, wait for a consumer to empty */
        {
            pthread_mutex_lock(&buff_size_mutex);
            pthread_cond_wait(&buff_full, &buff_size_mutex);   /* Wait for a consumer to read */
            pthread_mutex_unlock(&buff_size_mutex);
        }
    }
}

/* Print buffer to fp */
void* out_worker(void * void_fp)
{
    /* Detach thread */
    pthread_detach(pthread_self());

    FILE* fp = (FILE*) void_fp;

    int current_slot = 0;
    int id = fp == stdout ? 0 : 1;

    while (1)
    {
        pthread_mutex_lock(&buff_size_mutex);
        
        /* if buffer is empty, wait */
        while (buff_size == 0 && !done)
            pthread_cond_wait(&buff_empty, &buff_size_mutex);

#ifdef DEBUG
        printf("Outstream id %d at slot %d buff_size %d done %d buff_write %d prints: %s\n", id, current_slot, buff_size, done, buff_write, buffer[current_slot].str);
#endif
        pthread_mutex_unlock(&buff_size_mutex);

        if (buff_size == 0 && done) /* No more work */
            pthread_exit(NULL);

        /* Check against buffer bound */
        pthread_mutex_lock(&buff_write_mutex);

        while (current_slot == buff_write && !done)
            pthread_cond_wait(&buff_empty, &buff_write_mutex);

        if (current_slot == buff_write && done)
        {
            pthread_mutex_unlock(&buff_write_mutex);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&buff_write_mutex);

        /* More work to be done */
        fprintf(fp, "%s", buffer[current_slot].str);

        /* Update buffer bounds */
        pthread_mutex_lock(&buff_read_mutex);
        if (buffer[current_slot].read)
        {
            /* Both threads have read this, recycle */
            pthread_mutex_lock(&buff_size_mutex);
            buff_size--;

            /* If buff was full, we can signal here */
            pthread_cond_signal(&buff_full);

            pthread_mutex_unlock(&buff_size_mutex);
#ifdef DEBUG
            printf("Outstream id %d at slot %d decrements buff_size to %d\n", id, current_slot, buff_size);
#endif
        }
        else
            buffer[current_slot].read = 1;

        pthread_mutex_unlock(&buff_read_mutex);

        /* Move individual read slot */
        current_slot = (current_slot + 1) % MAX_BUFFER;
    }
}
