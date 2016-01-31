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
 * Last modified: 31/01-2016
 */ 

/* Fetch all the right functions */
#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 10
#define MAX_LENGTH 200

/* Thread task functions */
void* in_stream(void*);
void* out_stream(void*);

/* Circular buffer to hold string from standard input to be handled */
typedef struct {
    char* str;
} buff_slot;

buff_slot buffer[MAX_BUFFER_SIZE];

/* Define stuff, accessed with 'buff_size_mutex' */
int buff_size[2] = {0,0};

/* Locks and condition variables */
pthread_mutex_t buff_size_mutex;
pthread_cond_t buff_full, buff_empty;

/* Indicator set by in_stream to indicate end of stream */
int done = 0;

/* Number of output streams */
int out_streams = 2;

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
    else   /* Correct number of out_streams, as no filename given */
        out_streams = 1;

    /* Init buffer */
    for (i = 0; i < MAX_BUFFER_SIZE; i++)
        buffer[i].str = (char *) malloc(sizeof(char)*MAX_LENGTH);

    /* Start a thread to read from standard input */
    pthread_create(&tid, NULL, in_stream, NULL);

    /* Start a thread to write to file */
    if (fp != NULL)
        pthread_create(&tid, NULL, out_stream, (void *) fp);

    /* Let main thread do output to standard output */
    out_stream((void *)stdout);

    /* out_stream calls pthread_exit */
}

/* Helper function for in_stream, makes code easier to read and handles
 * semantics differences for one and two output streams */
int max_buff()
{
    if (out_streams == 1)
        return buff_size[0];
    else
        return buff_size[0] > buff_size[1] ?  buff_size[0] : buff_size[1];
}

/* Read stdin and store in buffer */
void* in_stream(void* arg)
{
    /* Detach thread */
    pthread_detach(pthread_self());

    /* Keep track of which slot to write to */
    int buff_write = 0;

    /* Functionaliy */
    while (1)
    {
        /* Wait until there's room in the buffer */
        pthread_mutex_lock(&buff_size_mutex);

        /* Buffer is full, wait for consumer signal */
        while (max_buff() == MAX_BUFFER_SIZE)
        {
#ifdef DEBUG
            printf("Instream waiting buff[0] %d  buff[1] %d max_buff %d buff_full\n",
                    buff_size[0], buff_size[1], max_buff());
#endif
            pthread_cond_wait(&buff_full, &buff_size_mutex);
        }
        pthread_mutex_unlock(&buff_size_mutex);

        /* Read new data */
        if (fgets(buffer[buff_write].str, MAX_LENGTH, stdin) != NULL)
        {
#ifdef DEBUG
            printf("Instream at slot %d size %d puts %s",
                    buff_write, max_buff(), buffer[buff_write].str);
#endif
            /* Update buffer bounds */
            pthread_mutex_lock(&buff_size_mutex);
            buff_size[0]++;
            
            /* Might not be writing to file */
            if (out_streams == 2)
                buff_size[1]++;

            buff_write = (buff_write+1) % MAX_BUFFER_SIZE;

            /* Signal both consumers */
            pthread_cond_broadcast(&buff_empty);
            pthread_mutex_unlock(&buff_size_mutex);
        }
        if (feof(stdin))    /* Stream is closed */
        {
#ifdef DEBUG
            printf("Instream is ended\n");
#endif
            /* Signal both consumers to exit if done */
            done = 1;   /* This is ok w/o lock */
            pthread_cond_broadcast(&buff_empty);
            pthread_exit(NULL);
        }
    }
}

/* Read buffer and write to file specified by argument */
void* out_stream(void* void_fp)
{
    /* Detach thread */
    pthread_detach(pthread_self());

    FILE* fp = (FILE*) void_fp;

    int current_slot = 0;
    int id = fp == stdout ? 0 : 1;

    while (1)
    {
        pthread_mutex_lock(&buff_size_mutex);

        /* Wait for done or work, both are signaled from in_stream */
        if (buff_size[id] == 0 && !done)
        {
#ifdef DEBUG
            printf("Outstream id %d at slot %d size %d waiting buff_empty\n",
                    id, current_slot, buff_size[id]);
#endif
            pthread_cond_wait(&buff_empty, &buff_size_mutex);
        }

        pthread_mutex_unlock(&buff_size_mutex);

        /* Is done? */
        if (buff_size[id] == 0 && done)
        {
#ifdef DEBUG
            printf("Outstream id %d exiting\n", id);
#endif
            fclose(fp);
            pthread_exit(NULL);
        }

#ifdef DEBUG
        printf("Outstream id %d at slot %d size %d print %s\n",
                id, current_slot, buff_size[id], buffer[current_slot].str);
#endif
        /* Aparently there is work to be done */
        fprintf(fp, "%s", buffer[current_slot].str);

        /* Update bounds */
        pthread_mutex_lock(&buff_size_mutex);
        buff_size[id]--;

#ifdef DEBUG
        printf("Outstream id %d at slot %d decrement buff_size to %d\n",
                id, current_slot, buff_size[id]);
#endif
        /* Signal instream ther is space in buffer */
        pthread_cond_signal(&buff_full);
        pthread_mutex_unlock(&buff_size_mutex);

        /* Update own bound */
        current_slot = (current_slot+1) % MAX_BUFFER_SIZE;
    }
}
