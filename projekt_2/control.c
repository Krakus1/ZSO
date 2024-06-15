#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include "ball.h"

#define PIPE_NAME "/tmp/ball_pipe"

void* control_func(void* arg) {
    int pipe_fd;
    char buffer[256];

    mkfifo(PIPE_NAME, 0666);

    while (1) {
        printf("Waiting for control command...\n");
        pipe_fd = open(PIPE_NAME, O_RDONLY);
        read(pipe_fd, buffer, sizeof(buffer));
        close(pipe_fd);

        int target_process;
        sscanf(buffer, "%d", &target_process);

        if (target_process >= 0 && target_process < NUM_PROCESSES) {
            kill(pids[target_process], SIGUSR1);
        }
    }

    return NULL;
}

void handle_signal(int sig) {
    int ball_stats[MAX_ROUNDS] = {0};

    pthread_mutex_lock(&shm->mutex);
    for (int i = 0; i < shm->count; ++i) {
        ball_stats[shm->balls[i].rounds]++;
    }
    pthread_mutex_unlock(&shm->mutex);

    int pipe_fd = open(PIPE_NAME, O_WRONLY);
    char buffer[256];
    sprintf(buffer, "Process %d stats:\n", getpid());
    for (int i = 0; i < MAX_ROUNDS; ++i) {
        sprintf(buffer + strlen(buffer), "Round %d: %d balls\n", i, ball_stats[i]);
    }
    write(pipe_fd, buffer, strlen(buffer));
    close(pipe_fd);
}

void* process_func(void* arg) {
    int process_id = (intptr_t)arg;
    signal(SIGUSR1, handle_signal);

    while (1) {
        sem_wait(&shm->sem);

        pthread_mutex_lock(&shm->mutex);
        for (int i = 0; i < shm->count; ++i) {
            if (shm->balls[i].to == process_id) {
                shm->balls[i].rounds++;
                if (shm->balls[i].rounds >= MAX_ROUNDS) {
                    printf("Process %d removed ball %d\n", process_id, shm->balls[i].ball_id);
                    shm->balls[i] = shm->balls[--shm->count];
                } else {
                    shm->balls[i].from = process_id;
                    shm->balls[i].to = (process_id + 1) % NUM_PROCESSES;
                    sem_post(&shm->sem);
                }
                break;
            }
        }
        pthread_mutex_unlock(&shm->mutex);
    }
}

