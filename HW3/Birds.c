#ifndef _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>
#include <unistd.h>

// Can be removed possibly
#include <sys/time.h>
#include <time.h>
double start_time, end_time;

#define NUMTHREADS 10 // Max amount of working threads (baby-birds)
#define MAXSIZE 20    // Max worm supply size
#define NUMCYCLES 3   // Max number of cycles the parent bird will fill the dish for
// #define DEBUG

int numBabyBird, worms, totalWorms, cycles, cycleCounter;

sem_t wormsMutex; // Mutual exclusion for pot
sem_t empty;      // Signals food is empty
sem_t full;       // Signals food is full

bool done = false; // Termination flag
bool printed = false;

void sleep_ms(int milliseconds)
{
    // Convert milliseconds to microseconds
    usleep(milliseconds * 1000);
}

void *babyBirdEat(void *arg)
{
    int id = *(int *)arg; // thread ID

    while (!done)
    {
        sem_wait(&full);       // wait until food is empty or being filled
        sem_wait(&wormsMutex); // exclusive access to eat

        if (done)
        { // If prior thread emptied the food capacity while holding the lock we terminate
            if (!printed)
            {
                printf("All baby birds are released from their eternal prison\n");
                printed = true;
            }
            sem_post(&wormsMutex);
            sem_post(&empty);
            break;
        }

        if (worms > 0)
        {
            worms--; // Consume worms
            printf("Baby bird %d ate a worm! Worms left:(%d/%d)\n", id, worms, totalWorms);
            sleep_ms(500);   // Simulate eating
            sem_post(&full); // Allow other birds to eat
        }
        else
        {
            if (cycleCounter <= cycles)
            {
                printf("Baby bird %d wakes up the parent bird!\n", id);
            }
            sem_post(&empty);
        }

        sem_post(&wormsMutex); // Release worms lock
    }

    return NULL;
}

void *parentBirdFill(void *arg)
{
    while (cycleCounter <= cycles)
    {
        cycleCounter++;
        sem_wait(&empty);      // Wait until food is empty
        sem_wait(&wormsMutex); // Exclusive access to dish

        printf("The parent bird flies out to find food!\n", worms);
        sleep(1); // Simulate filling up with food

        if (cycleCounter <= cycles)
        {
            worms = totalWorms;
            printf("The parent bird fills up the dish with worms! (%d portions)\n", worms);
        }
        sleep(1);
        sem_post(&wormsMutex); // Release worm lock
        sem_post(&full);       // Allow baby birds to eat
    }

    // Set done flag and release any waiting baby birds
    printf("The worm supply has run dry\n");
    sleep(1);
    done = true;
    for (int i = 0; i < numBabyBird; i++)
        sem_post(&full);

    return NULL;
}

int main(int argc, char *argv[])
{

    // Initialize variables
    sem_init(&wormsMutex, 0, 1); // Worms lock
    sem_init(&empty, 0, 1);      // Dish empty -> baby bird calls on parent bird
    sem_init(&full, 0, 0);       // Dish full -> baby birds eat

    /* read command line args if any */
    numBabyBird = (argc > 1) ? atoi(argv[1]) : NUMTHREADS;
    totalWorms = (argc > 2) ? atoi(argv[2]) : MAXSIZE;
    cycles = (argc > 3) ? atoi(argv[3]) : NUMCYCLES;
    if (numBabyBird > NUMTHREADS)
        numBabyBird = NUMTHREADS;
    if (totalWorms > MAXSIZE)
        totalWorms = MAXSIZE;
    if (cycles > NUMCYCLES)
        cycles = NUMCYCLES;

    pthread_t babyBird[numBabyBird], parentBird_thread;
    int ids[numBabyBird]; // Array to safely store thread IDs

    for (int i = 0; i < numBabyBird; i++)
    {
        ids[i] = i;
        pthread_create(&babyBird[i], NULL, babyBirdEat, &ids[i]);
    }
    pthread_create(&parentBird_thread, NULL, parentBirdFill, NULL);

    pthread_join(parentBird_thread, NULL); // Wait for parent bird to finish
    for (int i = 0; i < numBabyBird; i++)
        pthread_join(babyBird[i], NULL); // Wait for all baby birds to finish

    printf("The baby birds all passed from their worm-greed\n");
    printf("Simulation finished after %d cycles.\n", NUMCYCLES);
    return 0;
}
