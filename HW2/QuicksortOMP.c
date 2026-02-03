#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <omp.h>

#define NUMTHREADS 4         /* default number of workers */
#define MAXSIZE 100          /* maximum array size */
#define MAXTASKQUEUE 10000000 /* Size of the task queue */
//#define DEBUG

typedef struct
{
    int low;
    int high;
} Task;

omp_lock_t lock_task;
double start_time, end_time; // start and end times
int size;                    // assume size is multiple of numWorkers
int *arr;                    // Dynamic array
int numWorkers;              // number of workers

Task taskQueue[MAXTASKQUEUE];
volatile int taskCount = 0;     // Number of tasks in queue
volatile int activeThreads = 0; // Number of threads working on task
volatile bool done = false;     // Tells tasks to exit

void swap(int *x, int *y);
int partition(int low, int high);
void Worker();
void push_task(int low, int high);
void printArr();

void push_task(int low, int high)
{
    if (low >= high)
        return;

    omp_set_lock(&lock_task);
    taskQueue[taskCount].low = low;
    taskQueue[taskCount].high = high;
    taskCount++;
    omp_unset_lock(&lock_task);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    /* read command line args if any */
    size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2) ? atoi(argv[2]) : NUMTHREADS;

    /* Initialize OMP Lock */
    omp_init_lock(&lock_task);

    arr = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
        arr[i] = rand() % 100;

#ifdef DEBUG
    printf("Initial Array: ");
    printArr();
#endif

    start_time = omp_get_wtime();

    // Seed first task from array (whole array)
    push_task(0, size - 1);

    // Create omp_threads
    omp_set_num_threads(numWorkers);

#pragma omp parallel
    {
        Worker();
    }

    end_time = omp_get_wtime();

#ifdef DEBUG
    printf("Sorted Array:  ");
    printArr();
#endif

    printf("Sorted %d elements with %d threads in %g sec\n", size, numWorkers, end_time - start_time);
    free(arr);
    omp_destroy_lock(&lock_task);
    return 0;
}

void printArr()
{
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void Worker()
{
    int threadId = omp_get_thread_num();

    while (true)
    {
        Task myTask;

        // Thread acquires lock to check queue
        omp_set_lock(&lock_task);

        // If queue is empty, wait to check if we are done
        while (taskCount == 0)
        {
            // If there are no active threads, set done
            if (activeThreads == 0)
            {
                done = true;
            }

            if (done)
            {
                omp_unset_lock(&lock_task);
                return;
            }

            // If not done, release lock to let threads add tasks
            omp_unset_lock(&lock_task);

            // Re-acquire lock to check queue again
            omp_set_lock(&lock_task);
        }

        // Grab task
        if (taskCount > 0)
        {
            myTask = taskQueue[--taskCount];
            activeThreads++;
        }

        omp_unset_lock(&lock_task);

#ifdef DEBUG
        printf("Thread %ld started task: [%d, %d]\n", threadId, myTask.low, myTask.high);
#endif

        // QuickSort Algorithim

        // Partition returns the pivot index.
        int pivotIndex = partition(myTask.low, myTask.high);

        // Push new ranges to queue
        push_task(myTask.low, pivotIndex - 1);
        push_task(pivotIndex + 1, myTask.high);

        // Finish work
        // Decrement active worker count
        omp_set_lock(&lock_task);
        activeThreads--;
        omp_unset_lock(&lock_task);
    }
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