#ifndef _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#define MAXSIZE 10 /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */
#define DEBUG
#define NUM_CYCLES 5   /* number of times the bear eats honey before stopping */

int numHoneyBees;           /* number of workers */ 
int honeyPotCapacity;
int honeyPot;
double start_time, end_time; /* start and end times */
int cycles = 0;              /* counter for bear eating cycles */

sem_t potMutex;      // mutual exclusion for pot
sem_t empty;         // signals "pot is empty, bees may fill"
sem_t full;          // signals "pot is full, bear may eat"

bool done = false;   // termination flag

void *bee(void *arg){
    int id = *(int *)arg;  // thread ID

    while (!done) {
        sem_wait(&empty);       // wait until pot is empty or being filled
        sem_wait(&potMutex);    // exclusive access to pot

        if (done) {             // If the thread before filled up the pot while holding the lock we terminate
            sem_post(&potMutex);
            sem_post(&empty);
            break;
        }

        honeyPot++;             // add honey
        printf("Bee %d added honey (%d/%d)\n", id, honeyPot, honeyPotCapacity);

        if (honeyPot == honeyPotCapacity){
            printf("Bee %d wakes up the bear!\n", id);
            sem_post(&full);    // signal bear
        } else {
            sem_post(&empty);   // allow other bees to add
        }

        sem_post(&potMutex);    // release pot lock
        sleep(1);               // simulate work
    }

    return NULL;
}

void *bear(void *arg) {
    while (cycles < NUM_CYCLES) {
        sem_wait(&full);         // wait until pot is full
        sem_wait(&potMutex);     // exclusive access to pot

        printf("The Bear eats all the honey (%d portions)\n", honeyPot);
        honeyPot = 0;
        cycles++;

        sem_post(&potMutex);     // release pot lock
        sem_post(&empty);        // allow bees to refill
        sleep(1);                // simulate eating
    }

    // set done flag and release any waiting bees
    done = true;
    for (int i = 0; i < numHoneyBees; i++)
        sem_post(&empty);

    return NULL;
}

int main(int argc, char *argv[]){

    //Initialize variables
    sem_init(&potMutex, 0, 1); // pot unlocked
    sem_init(&empty, 0, 1);    // pot initially empty -> bees allowed
    sem_init(&full, 0, 0);     // pot not full -> bear sleeps

    /* read command line args if any */
    honeyPotCapacity = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    numHoneyBees = (argc > 2)? atoi(argv[2]) : MAXWORKERS; 
    if (honeyPotCapacity > MAXSIZE) honeyPotCapacity = MAXSIZE;
    if (numHoneyBees > MAXWORKERS) numHoneyBees = MAXWORKERS;

    honeyPot = 0; // the amount of portions in the honeypot
    pthread_t bees[numHoneyBees], bear_thread;
    int ids[numHoneyBees];   // array to safely store thread IDs

    pthread_create(&bear_thread, NULL, bear, NULL);
    for (int i = 0; i < numHoneyBees; i++){
        ids[i] = i;
        pthread_create(&bees[i], NULL, bee, &ids[i]);
    }
    
    pthread_join(bear_thread, NULL);     // wait for bear to finish

    for (int i = 0; i < numHoneyBees; i++)
        pthread_join(bees[i], NULL);    // wait for all bees to finish

    printf("The Bear has now had enough pots of Honey\n");
    printf("Simulation finished after %d cycles.\n", NUM_CYCLES);
    return 0;
}
