/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c 
     ./matrixSum-openmp size numWorkers

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size; 
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

typedef struct matrixElement {
  int value;
  int row;
  int col;
} matrixElement;

struct matrixElement min, max;

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  srand(time(NULL));
  int i, j, total=0;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  
  /* initialize the matrix */
  for (i = 0; i < size; i++) {
    //printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      //printf(" %d", matrix[i][j]);
	  }
    //printf(" ]\n");
  }
  
  start_time = omp_get_wtime();

  //Sätt min och max som första värdet i matrisen så vi kan jämföra de sen
  min.value = matrix[0][0];
  min.row = 0;
  min.col = 0;
  max.value = matrix[0][0]; 
  max.row = 0;
  max.col = 0;
  
  #pragma omp parallel 
  {
      struct matrixElement local_max; // thread-private
      struct matrixElement local_min;

      local_max.value = matrix[0][0];
      local_max.row = 0;
      local_max.col = 0;

      local_min.value = matrix[0][0];
      local_min.row = 0;
      local_min.col = 0;

      #pragma omp for reduction(+:total)
      for (i = 0; i < size; i++) {
          for (j = 0; j < size; j++){
              int val = matrix[i][j];
              if (val > local_max.value) {
                  local_max.value = val;
                  local_max.row = i;
                  local_max.col = j;
              }
              if (val < local_min.value) {
                  local_min.value = val;
                  local_min.row = i;
                  local_min.col = j;
              }
              total += val;
          }
      }

      #pragma omp critical
      {
          if (local_max.value > max.value) max = local_max;
          if (local_min.value < min.value) min = local_min;
      }
  }

// implicit barrier
  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("The min value in the matrix is %d in row %d and col %d\n", min.value, min.row + 1, min.col + 1);
  printf("The max value in the matrix is %d in row %d and col %d\n", max.value, max.row + 1, max.col + 1);
  printf("it took %g seconds\n", end_time - start_time);

}

