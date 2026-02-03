#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

void swap(int *x, int *y);
void quicksort(int *arr, int length);
void quicksort_recursion(int *arr, int low, int high);
int partition(int *arr, int low, int high);
void printArr(int *arr, int length);

double start_time, end_time;

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

int main()
{
    int arr[100000];
    int i;
    srand(time(NULL));

    for (i = 0; i < 100000; i++)
    {
        arr[i] = rand() % 100;
    }
    size_t length = sizeof(arr) / sizeof(int);

    //printArr(arr, length);
    start_time = read_timer();
    quicksort(arr, length);
    end_time = read_timer();
    //printArr(arr, length);
    printf("%g sec\n", end_time - start_time);

    return 0;
}

void printArr(int *arr, int length)
{
    for (int i = 0; i < length; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void quicksort(int *arr, int length)
{
    quicksort_recursion(arr, 0, length - 1);
}

void quicksort_recursion(int *arr, int low, int high)
{
    if (low < high)
    {
        int pivot_index = partition(arr, low, high);
        quicksort_recursion(arr, low, pivot_index - 1);
        quicksort_recursion(arr, pivot_index + 1, high);
    }
}

int partition(int *arr, int low, int high)
{
    int pivot_value = arr[high];

    int i = low;

    for (int j = low; j < high; j++)
    {
        if (arr[j] <= pivot_value)
        {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }

    swap(&arr[i], &arr[high]);

    return i;
}
