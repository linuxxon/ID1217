/* diff.c - multi threaded simple implementation of the UNIX diff program
 *
 * Homework 1 - Problem 5: The Unix diff command (20 points)
 *
 * ### PROGRAM DOES STUFF
 *
 * Semantics:
 *  See full description in README
 *
 * Usage:
 *  diff file1 file2
 *
 * Compilation:
 *  gcc diff.c -lpthread
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
#include <string.h>

#define MAX_BUFFER_SIZE 10
#define MAX_LENGTH 200

#define BUFF_WRITE linenumber % MAX_BUFFER_SIZE

/* Thread task functions */
void* in_stream(void*);
void* out_stream(void*);

/* Circular buffer to hold string from standard input to be handled */
typedef struct {
    int linenumber;
    char* filename;
    char* str;
} buff_slot;

buff_slot buffer1[MAX_BUFFER_SIZE];
buff_slot buffer2[MAX_BUFFER_SIZE];

/* Define stuff, accessed with 'buff_size_mutex' */
int buff1_size = 0;
int buff2_size = 0;

/* Locks and condition variables */
pthread_mutex_t buff_size_mutex;
pthread_cond_t buff_full;
pthread_cond_t buff_empty;

/* Arguments for instream */
typedef struct {
    int id;
    FILE* fp;
    char* filename;
} In_arg;

/* Indicator set by in_stream to indicate end of stream */
int done = 0;

/* Print standard input to filename provided and to standard output */
int main(int argc, char* argv[])
{
    int i;
    In_arg arg1, arg2;
    pthread_t tid;

    /* Read command line arguments */
    if (argc > 2)
    {
        arg1.id = 1;
        arg1.filename = argv[1];
        arg1.fp = fopen(arg1.filename, "r");

        if (arg1.fp == NULL)
        {
            fprintf(stderr, "Can't open file %s for reading", arg1.filename);
            exit(1);
        }

        arg2.id = 2;
        arg2.filename = argv[2];
        arg2.fp = fopen(arg2.filename, "r");
        if (arg2.fp == NULL)
        {
            fprintf(stderr, "Can't open file %s for reading", arg2.filename);
            fclose(arg1.fp);
            exit(1);
        }
    }
    else {
        fprintf(stderr, "No agruments dummy!\nUsage: diff file1 file2");
        exit(2);
    }

    /* Init buffers */
    for (i = 0; i < MAX_BUFFER_SIZE; i++) {
        buffer1[i].str = (char *) malloc(sizeof(char)*MAX_LENGTH);
        buffer1[i].filename = arg1.filename;
        buffer2[i].str = (char *) malloc(sizeof(char)*MAX_LENGTH);
        buffer2[i].filename = arg2.filename;
    }

    /* Start a thread to read from file1 */
    pthread_create(&tid, NULL, in_stream, (void*)&arg1);

    /* Start a thread to read from file2 */
    pthread_create(&tid, NULL, in_stream, (void*)&arg2);

    /* Let main thread do comparisons */
    out_stream(NULL);

    /* out_stream calls pthread_exit */
}

/* Read file fp and store in buffer */
void* in_stream(void* void_arg)
{
    /* Detach thread */
    pthread_detach(pthread_self());

    /* Convert void argument pointer */
    In_arg* arg = (In_arg*)void_arg;

    /* Keep track of which slot to write to */
    buff_slot* my_buffer = (arg->id == 1) ? buffer1 : buffer2;
    int* my_size = (arg->id == 1) ? &buff1_size : &buff2_size;

    long linenumber = 0;

    /* Functionaliy */
    while (1)
    {
        /* Wait until there's room in the buffer */
        pthread_mutex_lock(&buff_size_mutex);

        /* Buffer is full, wait for consumer signal */
        if (*my_size == MAX_BUFFER_SIZE)
            pthread_cond_wait(&buff_full, &buff_size_mutex);
        pthread_mutex_unlock(&buff_size_mutex);

        /* Read new data */
        if (fgets(my_buffer[BUFF_WRITE].str, MAX_LENGTH, arg->fp) != NULL)
        {
#ifdef DEBUG
            printf("Instream %d at slot %d size %d puts %s",
                    arg->id, BUFF_WRITE, *my_size, my_buffer[BUFF_WRITE].str);
#endif

            my_buffer[BUFF_WRITE].linenumber = linenumber;

            /* Update buffer bounds */
            pthread_mutex_lock(&buff_size_mutex);
            (*my_size)++;

            linenumber++;

            /* Signal comparer */
            pthread_cond_broadcast(&buff_empty);
            pthread_mutex_unlock(&buff_size_mutex);
        }
        if (feof(arg->fp))    /* Stream is closed */
        {
#ifdef DEBUG
            printf("Instream %d is ended\n", arg->id);
#endif

            /* Only used once so reuse size mutex */
            pthread_mutex_lock(&buff_size_mutex); 
            done++;   /* This is ok w/o lock */
            pthread_mutex_unlock(&buff_size_mutex);

            /* Signal comparer that a worker is done */
            pthread_cond_broadcast(&buff_empty);
            pthread_exit(NULL);
        }
    }
}

/* Read buffer one and two and print if they differ */
void* out_stream(void* arg)
{
    /* Detach thread */
    pthread_detach(pthread_self());

    int current_slot = 0;

    while (1)
    {
        pthread_mutex_lock(&buff_size_mutex);

        /* Not enough data in buffer, and noone is done*/
        while ( (buff1_size == 0 || buff2_size == 0) && !done)
        {
#ifdef DEBUG
            printf("Outstream at slot %d done %d 1 %d 2 %d waiting buff_empty\n",
                    current_slot, done, buff1_size, buff2_size);
#endif
            pthread_cond_wait(&buff_empty, &buff_size_mutex);
        }

        pthread_mutex_unlock(&buff_size_mutex);

        /* Is done? */
        if (buff1_size == 0 && buff2_size == 0 && done == 2)
        {
#ifdef DEBUG
            printf("Outstream exiting\n");
#endif
            pthread_exit(NULL);
        }

#ifdef DEBUG
        printf("Outstream at slot %d\n", current_slot);
#endif

        /* Comparison left to do */
        if (buff1_size != 0 && buff2_size != 0)
        {
            /* Not the same!! */
            if (strcmp(buffer1[current_slot].str, buffer2[current_slot].str))
                printf("%s line %ld\n%s\n%s line %ld\n%s\n",
                        buffer1[current_slot].filename,
                        buffer1[current_slot].linenumber, buffer1[current_slot].str,
                        buffer2[current_slot].filename,
                        buffer2[current_slot].linenumber, buffer2[current_slot].str);
            /* Do nothing otherwise */
        }
        /* One is done print the rest of the other */
        else if (buff1_size != 0 && done)
            printf("%s line %ld\n%s",
                    buffer1[current_slot].filename,
                    buffer1[current_slot].linenumber, buffer1[current_slot].str);
        else
            printf("%s line %ld\n%s",
                    buffer2[current_slot].filename,
                    buffer2[current_slot].linenumber, buffer2[current_slot].str);

        /* Update bounds */
        pthread_mutex_lock(&buff_size_mutex);
        if (buff1_size)
            buff1_size--;
        if (buff2_size)
            buff2_size--;

#ifdef DEBUG
        printf("Outstream at slot %d decrement buff1_size to %d buff2_size to %d\n",
                current_slot, buff1_size, buff2_size);
#endif
        /* Signal instream ther is space in buffer */
        pthread_cond_signal(&buff_full);
        pthread_mutex_unlock(&buff_size_mutex);

        /* Update own bound */
        current_slot = (current_slot+1) % MAX_BUFFER_SIZE;
    }
}
