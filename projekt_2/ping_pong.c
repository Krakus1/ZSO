#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "ball.h"

#define SHM_NAME "/ball_shm"
#define PIPE_NAME "/tmp/ball_pipe"
#define NUM_PROCESSES 4

SharedMemory *shm;

void handle_signal(int sig) {
    // Placeholder for signal handling
}

void create_shared_memory() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedMemory));
    shm = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    memset(shm, 0, sizeof(SharedMemory));
    pthread_mutex_init(&shm->mutex, NULL);
    sem_init(&shm->sem, 1, 0);
}

void delete_shared_memory() {
    munmap(shm, sizeof(SharedMemory));
    shm_unlink(SHM_NAME);
}

int main() {
    pid_t pids[NUM_PROCESSES];
    pthread_t control_thread;

    create_shared_memory();

    // Tworzenie procesów
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        if ((pids[i] = fork()) == 0) {
            process_func((void*)(intptr_t)i);
            exit(0);
        }
    }

    // Uruchomienie wątku kontrolnego
    pthread_create(&control_thread, NULL, control_func, NULL);

    // Wrzuć kilka piłeczek do systemu
    for (int i = 0; i < MAX_BALLS; ++i) {
        pthread_mutex_lock(&shm->mutex);
        if (shm->count < MAX_BALLS) {
            Ball ball = {i, 0, 0, 1};
            shm->balls[shm->count++] = ball;
            sem_post(&shm->sem);
        }
        pthread_mutex_unlock(&shm->mutex);
        sleep(1); // Daje czas na przetworzenie piłeczek
    }

    // Czekanie na zakończenie procesów
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    // Czekanie na zakończenie wątku kontrolnego
    pthread_join(control_thread, NULL);

    delete_shared_memory();
    return 0;
}
