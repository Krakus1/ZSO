#ifndef BALL_H
#define BALL_H

#include <pthread.h>
#include <semaphore.h>

#define MAX_BALLS 5
#define MAX_ROUNDS 5
#define NUM_PROCESSES 3

typedef struct {
    int ball_id;
    int rounds;
    int from;
    int to;
} Ball;

typedef struct {
    Ball balls[MAX_BALLS];
    int count;
    pthread_mutex_t mutex;
    sem_t sem;
} SharedMemory;

void* process_func(void* arg);
void* control_func(void* arg);
extern int logging_enabled;  // Zmienna globalna do kontrolowania logowania
extern pid_t pids[NUM_PROCESSES];  // Deklaracja zmiennej `pids` jako extern

void set_logging(int enable);

#endif // BALL_H
