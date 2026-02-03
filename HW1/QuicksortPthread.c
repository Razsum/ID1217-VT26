#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#define MAXWORKERS 4        /* maximum number of workers */
#define MAXSIZE 100         /* maximum array size */
#define MAXTASKQUEUE 100000 /* Size of the task queue */
//#define DEBUG

typedef struct
{
    int low;
    int high;
} Task;

pthread_mutex_t mutex_task; // Protects the task queue
pthread_cond_t cond_task;   // Signals when tasks are available

double start_time, end_time; /* start and end times */
int size;                    /* assume size is multiple of numWorkers */
int *arr;
int numWorkers; /* number of workers */

Task taskQueue[MAXTASKQUEUE];
int taskCount = 0;     // Number of tasks in queue
int activeThreads = 0; // Number of threads working on task
bool done = false;     // Tells tasks to exit

void swap(int *x, int *y);
int partition(int low, int high);
void *Worker(void *arg);
void push_task(int low, int high);
void printArr();
double read_timer();

/* timer */
double read_timer()
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if (!initialized)
    {
        gettimeofday(&start, NULL);
        initialized = true;
    }
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

void push_task(int low, int high)
{
    if (low >= high)
        return;

    pthread_mutex_lock(&mutex_task);
    taskQueue[taskCount].low = low;
    taskQueue[taskCount].high = high;
    taskCount++;
    pthread_cond_signal(&cond_task);

    pthread_mutex_unlock(&mutex_task);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    pthread_attr_t attr;
    pthread_t workerid[MAXWORKERS]; // array of thread id handlers

    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); // Anger hur trådar schemaläggs av operativsystemet

    /* initialize mutex and condition variable */
    pthread_mutex_init(&mutex_task, NULL);
    pthread_cond_init(&cond_task, NULL);

    /* read command line args if any */
    size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;          // Bestämmer storleken på arrayen som ska sorteras
    numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS; // Bestämmer antalet threads från den andra inputen i konsolen

    arr = (int *)malloc(size * sizeof(int));

    for (int i = 0; i < size; i++)
    {
        arr[i] = rand() % 100;
    }

#ifdef DEBUG
    printf("Initial Array: ");
    printArr();
#endif

    start_time = read_timer();

    // Seed first task from array (whole array)
    push_task(0, size - 1);

    for (long l = 0; l < numWorkers; l++)
        pthread_create(&workerid[l], &attr, Worker, (void *)l);

    for (long l = 0; l < numWorkers; l++)
    {
        pthread_join(workerid[l], NULL);
    }

    end_time = read_timer();
    printf("Sorted %d elements with %d workers in %g sec\n", size, numWorkers, end_time - start_time);

#ifdef DEBUG
    printf("Sorted Array:  ");
    printArr();
#endif

    free(arr);
    return 0;
}

void printArr()
{
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void *Worker(void *arg)
{
    long myid = (long)arg;

    while (true)
    {
        Task myTask;

        // Acquire task
        pthread_mutex_lock(&mutex_task);

        while (taskCount == 0 && !done)
        {
            // If queue empty and no threads are active, the sort is finished
            // Wake everyone and exit
            if (activeThreads == 0)
            {
                done = true;
                pthread_cond_broadcast(&cond_task);
                pthread_mutex_unlock(&mutex_task);
                pthread_exit(NULL);
            }
            pthread_cond_wait(&cond_task, &mutex_task);
        }

        // Check if done
        if (done)
        {
            pthread_mutex_unlock(&mutex_task);
            pthread_exit(NULL);
        }

        // Grab task
        myTask = taskQueue[--taskCount];
        activeThreads++;
        pthread_mutex_unlock(&mutex_task);
#ifdef DEBUG
        printf("Worker %ld started task: [%d, %d]\n", myid, myTask.low, myTask.high);
#endif

        // QuickSort Algorithim

        // Partition returns the pivot index.
        int pivotIndex = partition(myTask.low, myTask.high);

        // Push new ranges to queue
        push_task(myTask.low, pivotIndex - 1);
        push_task(pivotIndex + 1, myTask.high);

        // Finish work
        pthread_mutex_lock(&mutex_task);
        activeThreads--;
        pthread_mutex_unlock(&mutex_task);
    }
    return NULL;
}

void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

int partition(int low, int high)
{
    int pivotValue = arr[high];
    int i = low;

    for (int j = low; j < high; j++)
    {
        if (arr[j] <= pivotValue)
        {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }
    swap(&arr[i], &arr[high]);
    return i;
}