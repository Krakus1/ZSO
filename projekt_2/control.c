#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>
#include "ball.h"

#define PIPE_NAME "/tmp/ball_pipe"
#define SHM_NAME "/my_ball_shm"

extern SharedMemory *shm;
extern pid_t pids[NUM_PROCESSES];
int logging_enabled = 1;

void set_logging(int enable) {
    logging_enabled = enable;
    if (logging_enabled) {
        printf("Logging enabled\n");
    } else {
        printf("Logging disabled\n");
    }
}

void handle_signal(int sig) {
    int ball_stats[MAX_ROUNDS] = {0};

    pthread_mutex_lock(&shm->mutex);
    for (int i = 0; i < shm->count; ++i) {
        ball_stats[shm->balls[i].rounds]++;
    }
    pthread_mutex_unlock(&shm->mutex);

    int pipe_fd = open(PIPE_NAME, O_WRONLY);
    if (pipe_fd == -1) {
        perror("open pipe_fd in handle_signal");
        return;
    }
    char buffer[256];
    sprintf(buffer, "Process %d stats:\n", getpid());
    for (int i = 0; i < MAX_ROUNDS; ++i) {
        sprintf(buffer + strlen(buffer), "Round %d: %d balls\n", i, ball_stats[i]);
    }
    write(pipe_fd, buffer, strlen(buffer));
    close(pipe_fd);
}

void signal_handler(int sig) {
    if (sig == SIGUSR2) {
        set_logging(!logging_enabled);
    }
}

void* control_func(void* arg) {
    int pipe_fd;
    char buffer[256];

    if (mkfifo(PIPE_NAME, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(1);
    }

    signal(SIGUSR2, signal_handler);

    while (1) {
        if (logging_enabled) {
            printf("Waiting for control command...\n");
        }
        pipe_fd = open(PIPE_NAME, O_RDONLY);
        if (pipe_fd == -1) {
            perror("open pipe_fd");
            continue;
        }
        ssize_t num_read = read(pipe_fd, buffer, sizeof(buffer) - 1);
        if (num_read > 0) {
            buffer[num_read] = '\0';
            if (logging_enabled) {
                printf("Received command: %s\n", buffer);
            }

            int target_process;
            if (sscanf(buffer, "%d", &target_process) == 1) {
                if (target_process >= 0 && target_process < NUM_PROCESSES) {
                    if (logging_enabled) {
                        printf("Sending signal to process %d\n", target_process);
                    }
                    kill(pids[target_process], SIGUSR1);
                } else {
                    if (logging_enabled) {
                        printf("Invalid process number: %d\n", target_process);
                    }
                }
            } else {
                if (logging_enabled) {
                    printf("Invalid command format: %s\n", buffer);
                }
            }
        } else {
            perror("read");
        }
        close(pipe_fd);
    }

    return NULL;
}

void* process_func(void* arg) {
    int process_id = (intptr_t)arg;

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open in process_func");
        exit(1);
    }
    shm = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap in process_func");
        exit(1);
    }

    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, signal_handler);

    while (1) {
        if (logging_enabled) {
            printf("Process %d waiting for ball\n", process_id);
        }
        sem_wait(&shm->sem);
        sleep(1);

        pthread_mutex_lock(&shm->mutex);
        for (int i = 0; i < shm->count; ++i) {
            if (shm->balls[i].to == process_id) {
                if (logging_enabled) {
                    printf("Process %d received ball %d from process %d in round %d\n",
                           process_id, shm->balls[i].ball_id, shm->balls[i].from, shm->balls[i].rounds);
                }
                sem_post(&shm->sem);
                sleep(1);
                shm->balls[i].rounds++;
                if (shm->balls[i].rounds >= MAX_ROUNDS) {
                    if (logging_enabled) {
                        printf("Process %d removed ball %d in round %d\n", process_id, shm->balls[i].ball_id, shm->balls[i].rounds);
                    }
                    shm->balls[i] = shm->balls[--shm->count];
                    sem_post(&shm->sem);
                    sleep(1);
                } else {
                    shm->balls[i].from = process_id;
                    shm->balls[i].to = (process_id + 1) % NUM_PROCESSES;
                    if (logging_enabled) {
                        printf("Process %d passing ball %d to process %d in round %d\n",
                               process_id, shm->balls[i].ball_id, shm->balls[i].to, shm->balls[i].rounds);
                    }
                    sem_post(&shm->sem);  // Upewnijmy się, że semafor jest prawidłowo ustawiony
                    sleep(1);
                }
                break;
                sleep(1);
            }
        }
        pthread_mutex_unlock(&shm->mutex);
    }
}
