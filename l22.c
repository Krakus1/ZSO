#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#define test_errno(msg) do{if (errno) {perror(msg); exit(EXIT_FAILURE);}} while(0)

void koniec_watku(int licznik, int limit) {
    int i;
    for (i=0; i < licznik; i++) putchar('.');
        printf("licznik = %d, limit = %d", licznik, limit);
 
    if (licznik == limit)
        /* zakończenie wątku w którego kontekście wykonywana jest ta funkcja */
        pthread_exit(NULL);
    else
        koniec_watku(licznik+1, limit);
}

void* watek(void* arg) {
    koniec_watku(0, 5);
    return NULL;
}

int main() {
    pthread_t id;
 
    /* utworzenie wątku */
    errno = pthread_create(&id, NULL, watek, NULL);
    test_errno("pthread_create");
 
    /* oczekiwanie na jego zakończenie */
    errno = pthread_join(id, NULL);
    test_errno("pthread_join");
 
    return EXIT_SUCCESS;
}