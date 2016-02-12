/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c 
     ./matrixSum-openmp size numWorkers

     Problem 1.a)

    Author: Rasmus Linusson
    Last modified: 8/2-2016

    Coursework for ID1217 at KTH
*/

#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 12  /* maximum number of workers */

typedef struct {
    int value;
    int x;
    int y;
} Pos;

int numWorkers;
int size; 
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long total=0; /* Change to long, cuz overflow */
  Pos min, max;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  srand(7); /* Same seed as in hw1 */

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
    //  printf("[ ");
      for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      //      printf(" %d", matrix[i][j]);
      }
      //      printf(" ]\n");
  }

  /* Initialze min max */
  max.value = matrix[0][0];
  min.value = matrix[0][0];
  max.x = max.y = min.x = min.y = 0;
  start_time = omp_get_wtime();

#pragma omp parallel for reduction (+:total) private(j) shared(min,max)
  for (i = 0; i < size; i++)
  {
    for (j = 0; j < size; j++)
    {
      total += matrix[i][j];

      /* Check for min */
      if (min.value > matrix[i][j])
      {
#pragma omp critical (min)
          if (min.value > matrix[i][j])
          {
              min.value = matrix[i][j];
              min.x = j;
              min.y = i;
          }
      }

      /* Check for max */
      if (max.value < matrix[i][j])
      {
#pragma omp critical (max)
          if (max.value < matrix[i][j])
          {
              max.value = matrix[i][j];
              max.x = j;
              max.y = i;
          }
      }
    }
  } // implicit barrier

  end_time = omp_get_wtime();

  printf("the total is %ld, max is %d at [%d,%d], min is %d at [%d,%d]\n",
          total, max.value, max.x, max.y, min.value, min.x, min.y);
  printf("it took %g seconds\n", end_time - start_time);

}
