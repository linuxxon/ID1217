/* palindrome.c - multi-threaded program to check a wordlist for palindromic
 * words
 *
 * Homework 2 - Problem 3: Find Palindromic Words (40 points)
 *
 * Semantics:
 *  See full description in README
 *
 * Usage:
 *  ./palindrome path/to/dict
 *
 * Compilation:
 *  gcc -fopenmp palindrome.c 
 *
 * For course ID1217 at KTH
 *
 * Author: Rasmus Linusson
 * Last modified: 12/02-2016
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <omp.h>

#define MAX_WORDS 32000
#define MAX_WORD_LENGTH 128
#define MAX_THREADS 12

#define OUTFILE "palindromes.res"

double start_time, end_time;

int num_words = 0;
int num_palindroms = 0;

char word_buff[MAX_WORD_LENGTH];
char* dictionary[MAX_WORDS];

/* Pointers to original strings in dictionary */
char* found_palindroms[MAX_WORDS];

/* Helper function */
int is_palindrome(char* str1, char* str2, int length1, int length2);

int main(int argc, char* argv[])
{
    int i=0, j=0;
    int num_threads = MAX_THREADS;
    char* filename = NULL;

    if (argc > 1)
        filename = argv[1];
    if (argc > 2)
        num_threads = atoi(argv[2]);

    omp_set_num_threads(num_threads);

    /* Init dictionary */
    FILE* fp = fopen(filename, "r");
    FILE* out_fp = fopen(OUTFILE, "w");
    
    /* No file */
    if (!fp) {
        fprintf(stderr, "Can't open file %s. Exiting", filename);
        exit(1);
    }
    if (!out_fp) {
        fprintf(stderr, "Can't open file %s. Exiting", OUTFILE);
        exit(1);
    }

    /* Load words */
    while (fscanf(fp, "%s", word_buff))
    {
        int word_length = strlen(word_buff);
        dictionary[num_words] = malloc(sizeof(char)*word_length);
        memcpy(dictionary[num_words++], word_buff, word_length);
        
        if (feof(fp))
            break;
    }
    fclose(fp);

    /* Record time */
    start_time = omp_get_wtime();

    /* Do stuff in parallell */
#pragma omp parallel for private(i,j)
    for (i = 0; i < num_words; i++)
    {
        /* Got a word! Word is located at dictionary[i] */

        int word_length_i = strlen(dictionary[i]);
        int palindrome = 0;

        /* Compare backwards */
        if (is_palindrome(dictionary[i], dictionary[i], word_length_i, word_length_i))
            palindrome = 1;

        /* Compare to all following words */
        for (j = i+1; j < num_words; j++)
        {
            if (is_palindrome(dictionary[i], dictionary[j], word_length_i, -1))
            {
                palindrome = 1;

                /* Add the other palindrome */
#pragma omp critical 
                found_palindroms[num_palindroms++] = dictionary[j];

                break;
            }
        }

        /* Add the word examined as a palindrom */
        if (palindrome) {
#pragma omp critical
            found_palindroms[num_palindroms++] = dictionary[i];
        }
    }

    /* Done, save time and stuff */
    end_time = omp_get_wtime();

    /* Report to user */
    printf("Found %d palindroms in %f seconds\n", num_palindroms, end_time-start_time);

    /* Print result to file */
    fprintf(out_fp,"Found %d palindroms\n", num_palindroms);
    i=0;
    while(i < num_palindroms)
        fprintf(out_fp, "%s\n", found_palindroms[i++]);

    /* Close outfile */
    fclose(out_fp);

    return 0;
}

/* Check to c-style strings for palindimicity
 *
 * Lengths can be provided, at own risk, to increase performance by avoiding
 * calculating the lengths of strings more than once
 *
 * Returns 0 if not palindromic
 * Returns != 0 if palindromic
 *
 */
int is_palindrome(char* str1, char* str2, int length1, int length2)
{
    /* If length was not provided, calculate */
    if (length1 == -1)
        length1 = strlen(str1);
    if (length2 == -1)
        length2 = strlen(str2);

    /* Can't be palindomic if different lengths */
    if (length1 == length2)
    { 
        int k;
        for (k = 0; k < length2; k++)
        {
            char a = tolower(str1[k]);
            char b = tolower(str2[length2-k-1]);

            /* Check if reverse of i is the same as the current word */
            if ((int)tolower(str1[k]) != (int)tolower(str2[length2-k-1]))
                return 0;
        }
#ifdef DEBUG
        printf("Seems like palindromic %s & %s\n", str1, str2);
#endif
        return 1;
    }
    else
        return 0; /* As in false */
}
