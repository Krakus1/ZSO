#ifndef BALL_H
#define BALL_H

#include <pthread.h>
#include <semaphore.h>

#define MAX_BALLS 10
#define MAX_ROUNDS 5

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

#endif // BALL_H
