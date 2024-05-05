#include <stdio.h>
#include <pthread.h>

// Funkcja wykonywana przez pierwszy wątek
void *thread_function1(void *arg) {
    int i;
    for (i = 0; i < 5; i++) {
        printf("Wątek 1: Iteracja %d\n", i);
        fflush(stdout);
        sleep(1); // Czekaj przez 1 sekundę
    }
    return NULL;
}

// Funkcja wykonywana przez drugi wątek
void *thread_function2(void *arg) {
    int i;
    for (i = 0; i < 3; i++) {
        printf("Wątek 2: Iteracja %d\n", i);
        fflush(stdout);
        sleep(2); // Czekaj przez 2 sekundy
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    
    // Tworzenie wątków
    pthread_create(&thread1, NULL, thread_function1, NULL);
    pthread_create(&thread2, NULL, thread_function2, NULL);

    // Oczekiwanie na zakończenie wątków
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    printf("Wątki zakończyły pracę.\n");
    
    return 0;
}
