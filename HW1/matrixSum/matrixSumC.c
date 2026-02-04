/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
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
#define MAXSIZE 10 /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */
#define DEBUG

pthread_mutex_t resultMutex, taskMutex, minMax;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */

typedef struct matrixElement {
  int value;
  int row;
  int col;
} matrixElement;

/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&resultMutex);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &resultMutex);
  pthread_mutex_unlock(&resultMutex);
}

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

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */
int total;
int rowCounter;
struct matrixElement min, max;

void *Worker(void *); //Här berättaar vi bara för C att functionen finns så main kan hitta den ändå om den ligger under main

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  srand(time(NULL));

  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS]; //array of thread id handlers

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); // Anger hur trådar schemaläggs av operativsystemet. Tråden konkurrerar om CPU med alla andra systemtrådar Varje pthread mappas till en riktig OS-tråd

  /* initialize mutex and condition variable */
  pthread_mutex_init(&resultMutex, NULL);
  pthread_mutex_init(&taskMutex, NULL);
  pthread_mutex_init(&minMax, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE; // Bestämmer bredd och höjd på matrixen som den första inputen i konsolen
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS; // Bestämmer antalet threads från den andra inputen i konsolen
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers; // antalet rows en thread ska hantera

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand()%99;
	  }
  }

  //Sätt min och max som första värdet i matrisen så vi kan jämföra de sen
  min.value = matrix[0][0];
  min.row = 0;
  min.col = 0;
  max.value = matrix[0][0]; 
  max.row = 0;
  max.col = 0;

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
  

  total = 0;
  rowCounter = 0;
  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);
  
  for (l = 0; l < numWorkers; l++)
    pthread_join(workerid[l], NULL);

  /* get end time */
  end_time = read_timer();
  /* print results */
  printf("The total is %d\n", total);
  printf("The min value in the matrix is %d in row %d and col %d\n", min.value, min.row + 1, min.col + 1);
  printf("The max value in the matrix is %d in row %d and col %d\n", max.value, max.row + 1, max.col + 1);
  printf("The execution time is %g sec\n", end_time - start_time);

}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;
  int row, j, threadTotal;
  struct matrixElement threadMin, threadMax;

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  while(1) {
    pthread_mutex_lock(&taskMutex);
      if (rowCounter >= size )  {
        pthread_mutex_unlock(&taskMutex);
        break; //No rows left, work is done
      }

      row = rowCounter;
      rowCounter++;
    pthread_mutex_unlock(&taskMutex);

    threadMax.value = matrix[row][0];
    threadMax.row = row;
    threadMax.col = 0;
    threadMin.value = matrix[row][0];
    threadMin.row = row;
    threadMin.col = 0;
    threadTotal = 0;
    
    for (j = 0; j < size; j++){
      if (threadMax.value < matrix[row][j]) {
        threadMax.value = matrix[row][j];
        threadMax.row = row;
        threadMax.col = j;
      } else if(threadMin.value > matrix[row][j]) {
        threadMin.value = matrix[row][j];
        threadMin.row = row;
        threadMin.col = j;
      }
      threadTotal += matrix[row][j];
    }
    pthread_mutex_lock(&minMax);
      if (max.value < threadMax.value){
        max.value = threadMax.value;
        max.row = threadMax.row;
        max.col = threadMax.col;
      } else if(min.value > threadMin.value) {
        min.value = threadMin.value;
        min.row = threadMin.row;
        min.col = threadMin.col;
      }
    pthread_mutex_unlock(&minMax);
    pthread_mutex_lock(&resultMutex);
      total += threadTotal;
    pthread_mutex_unlock(&resultMutex);
  }


  return NULL;
}