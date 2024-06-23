#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdint.h>
#include <sys/stat.h>
#include "ball.h"

#define SHM_NAME "/my_ball_shm"

SharedMemory *shm;
pid_t pids[NUM_PROCESSES];  // Definicja zmiennej `pids`

void create_shared_memory() {
    shm_unlink(SHM_NAME);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(shm_fd, sizeof(SharedMemory)) == -1) {
        perror("ftruncate");
        close(shm_fd);
        exit(1);
    }
    shm = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(1);
    }
    memset(shm, 0, sizeof(SharedMemory));
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shm->mutex, &mutex_attr);
    sem_init(&shm->sem, 1, 0);

    if (fchmod(shm_fd, 0666) == -1) {
        perror("fchmod");
        close(shm_fd);
        exit(1);
    }
    close(shm_fd);
}

void delete_shared_memory() {
    if (munmap(shm, sizeof(SharedMemory)) == -1) {
        perror("munmap");
    }
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }
}

int main() {
    pthread_t control_thread;

    create_shared_memory();

    // Ustawienie logowania na true lub false
    set_logging(1);  // Włączanie logowania
    // set_logging(0);  // Wyłączanie logowania

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        if ((pids[i] = fork()) == 0) {
            process_func((void*)(intptr_t)i);
            exit(0);
        }
    }

    pthread_create(&control_thread, NULL, control_func, NULL);

    for (int i = 0; i < MAX_BALLS; ++i) {
        
        pthread_mutex_lock(&shm->mutex);
        if (shm->count < MAX_BALLS) {
            Ball ball = {i, 0, 0, 1};
            shm->balls[shm->count++] = ball;
            sem_post(&shm->sem);
            if (logging_enabled) {
                printf("Added ball %d, total balls: %d\n", i, shm->count);
            }
        } else {
            if (logging_enabled) {
                printf("Maximum number of balls reached: %d\n", shm->count);
            }
            
        }
        sem_post(&shm->sem);
        pthread_mutex_unlock(&shm->mutex);
        sleep(1);
    }

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    pthread_join(control_thread, NULL);

    delete_shared_memory();
    return 0;
}
