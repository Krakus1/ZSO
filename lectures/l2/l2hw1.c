//Napisz aplikację, która utworzy wątki o strukturze odpowiadającej 
//pierwszemu schematowi przedstawionym na pierwszym bloku. 
//Każdy wątek jako argument ma dostać ilość sekund przez jaką będzie 
//istniał. Po uruchomieniu wątek ma wyświetlić informację na konsoli, 
//że żyje, odczekać zadaną ilość sekund, wyświetlić informację, że się 
//kończy i zwrócić godzinę, minutę i sekundę, o której się zakończył. 
//Przykładowo Wątek1 tworzy wątki Wątek3, Wątek4 i Wątek7 i czeka, aż 
//każdy z tych wątków się zakończy, a następnie sam odczekuje swoje 
//zadane ileś sekund. I tak działają wszystkie wątki.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// Struktura argumentów dla wątku
typedef struct {
    int thread_num;
    int sleep_time;
    int *child_threads;
    int *child_sleep_times;
    int num_children;
} thread_arg_t;

// Funkcja wykonywana przez wątek
void *thread_function(void *arg) {
    thread_arg_t *targ = (thread_arg_t *) arg;
    int thread_num = targ->thread_num;
    int sleep_time = targ->sleep_time;
    int *child_threads = targ->child_threads;
    int *child_sleep_times = targ->child_sleep_times;
    int num_children = targ->num_children;

    printf("Wątek %d: Żyję\n", thread_num);
    fflush(stdout);

    // Tworzenie pod-wątków, jeśli są
    if (num_children > 0) {
        pthread_t *threads = malloc(num_children * sizeof(pthread_t));
        thread_arg_t *args = malloc(num_children * sizeof(thread_arg_t));

        for (int i = 0; i < num_children; i++) {
            args[i].thread_num = child_threads[i];
            args[i].sleep_time = child_sleep_times[i];
            args[i].child_threads = NULL;  // Brak pod-wątków dla tego poziomu
            args[i].child_sleep_times = NULL;
            args[i].num_children = 0;
            pthread_create(&threads[i], NULL, thread_function, &args[i]);
        }

        // Oczekiwanie na zakończenie pod-wątków
        for (int i = 0; i < num_children; i++) {
            int *end_time;
            pthread_join(threads[i], (void **) &end_time);
            free(end_time);
        }

        free(threads);
        free(args);
    }

    // Symulacja pracy wątku
    sleep(sleep_time);

    // Pobranie aktualnego czasu
    time_t now;
    struct tm *now_tm;
    int hour, minute, second;
    time(&now);
    now_tm = localtime(&now);
    hour = now_tm->tm_hour;
    minute = now_tm->tm_min;
    second = now_tm->tm_sec;

    printf("Wątek %d: Kończę pracę o %02d:%02d:%02d\n", thread_num, hour, minute, second);
    fflush(stdout);

    // Zwrócenie aktualnego czasu
    int *end_time = malloc(3 * sizeof(int));
    end_time[0] = hour;
    end_time[1] = minute;
    end_time[2] = second;
    return (void *) end_time;
}

// Funkcja tworząca wątki zgodnie ze schematem
void create_threads(int thread_num, int sleep_time, int child_threads[], int child_sleep_times[], int num_children) {
    pthread_t thread;
    thread_arg_t arg = {thread_num, sleep_time, child_threads, child_sleep_times, num_children};
    pthread_create(&thread, NULL, thread_function, &arg);
    pthread_join(thread, NULL);
}

int main() {
    // Przykład struktury wątków
    // Wątek 1 tworzy wątki 3, 4, 7
    int thread1_children[] = {3, 4, 7};
    int thread1_sleep_times[] = {2, 2, 2};

    // Wątek 2 tworzy wątki 5, 6
    int thread2_children[] = {5, 6};
    int thread2_sleep_times[] = {2, 2};

    // Tworzenie i uruchamianie wątku 1
    create_threads(1, 2, thread1_children, thread1_sleep_times, 3);

    // Tworzenie i uruchamianie wątku 2
    create_threads(2, 2, thread2_children, thread2_sleep_times, 2);

    printf("Wszystkie wątki zakończyły pracę.\n");
    return 0;
}
