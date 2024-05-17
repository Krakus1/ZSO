#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_CARS 2
#define NUM_STATIONS 2
#define MIN_REFUEL 3

pthread_mutex_t stations_mutex[NUM_STATIONS];
pthread_cond_t stations_cond[NUM_STATIONS];
pthread_mutex_t helper_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t helper_cond = PTHREAD_COND_INITIALIZER;

typedef struct {
    int car_id;
    int fuel;
    int oil;
    int coolant;
    int refuels;
    int is_waiting;
} car_t;

car_t *waiting_car = NULL;

void* car_thread(void *arg) {
    car_t *car = (car_t *)arg;
    int needs_refuel = 0;

    printf("Samochód %d: Rozpoczynam jazdę\n", car->car_id);

    while (car->refuels < MIN_REFUEL) {
        sleep(1);

        // Symulacja zużycia płynów
        car->fuel -= rand() % 5 + 1;
        car->oil -= rand() % 5 + 1;
        car->coolant -= rand() % 5 + 1;

        if (car->fuel <= 0 || car->oil <= 0 || car->coolant <= 0) {
            needs_refuel = 1;

            // Zgłoszenie zapotrzebowania na tankowanie
            printf("Samochód %d: Brakuje płynów, potrzebuję stacji\n", car->car_id);

            // Szukanie dostępnej stacji
            int refueled = 0;
            for (int i = 0; i < NUM_STATIONS; i++) {
                pthread_mutex_lock(&stations_mutex[i]);
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += 2;

                printf("Samochód %d: Czekam na stację %d\n", car->car_id, i);
                int res = pthread_cond_timedwait(&stations_cond[i], &stations_mutex[i], &ts);
                if (res == 110) {
                    printf("Samochód %d: Korzystam ze stacji %d\n", car->car_id, i);

                    // Uzupełnienie płynów
                    car->fuel = car->fuel <= 0 ? 100 : car->fuel;
                    car->oil = car->oil <= 0 ? 100 : car->oil;
                    car->coolant = car->coolant <= 0 ? 100 : car->coolant;

                    car->refuels++;
                    refueled = 1;

                    printf("Samochód %d: Zatankowałem na stacji %d, refuels: %d\n", car->car_id, i, car->refuels);

                    pthread_cond_signal(&stations_cond[i]);
                    pthread_mutex_unlock(&stations_mutex[i]);
                    break;
                } else {
                    printf("Samochód %d: Nie udało się uzyskać dostępu do stacji %d, res: %d\n", car->car_id, i, res);
                }
                pthread_mutex_unlock(&stations_mutex[i]);
            }

            // Jeśli nie udało się zatankować, zgłaszamy awarię i czekamy na pomoc
            if (!refueled) {
                pthread_mutex_lock(&helper_mutex);
                waiting_car = car;
                car->is_waiting = 1;
                printf("Samochód %d: Awaria, czekam na pomoc\n", car->car_id);
                pthread_cond_wait(&helper_cond, &helper_mutex);
                car->is_waiting = 0;
                pthread_mutex_unlock(&helper_mutex);

                car->refuels++;
                printf("Samochód %d: Naprawiony przez pomoc, refuels: %d\n", car->car_id, car->refuels);
            }
        }
    }

    printf("Samochód %d: Zakończyłem jazdę\n", car->car_id);
    return NULL;
}

void* helper_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&helper_mutex);
        while (waiting_car == NULL) {
            pthread_cond_wait(&helper_cond, &helper_mutex);
        }

        car_t *car = waiting_car;
        printf("Samochód pomocy: Otrzymałem zgłoszenie od samochodu %d\n", car->car_id);
        sleep(2);

        // Uzupełnienie płynów
        car->fuel = 100;
        car->oil = 100;
        car->coolant = 100;

        printf("Samochód pomocy: Samochód %d zatankowany\n", car->car_id);
        waiting_car = NULL;
        pthread_cond_signal(&helper_cond);
        pthread_mutex_unlock(&helper_mutex);
    }
}

int main() {
    srand(time(NULL));

    pthread_t car_threads[NUM_CARS];
    pthread_t helper;
    car_t cars[NUM_CARS];

    // Inicjalizacja mutexów i zmiennych warunkowych
    for (int i = 0; i < NUM_STATIONS; i++) {
        pthread_mutex_init(&stations_mutex[i], NULL);
        pthread_cond_init(&stations_cond[i], NULL);
    }

    pthread_create(&helper, NULL, helper_thread, NULL);

    // Tworzenie wątków samochodów
    for (int i = 0; i < NUM_CARS; i++) {
        cars[i].car_id = i + 1;
        cars[i].fuel = 100;
        cars[i].oil = 100;
        cars[i].coolant = 100;
        cars[i].refuels = 0;
        cars[i].is_waiting = 0;

        pthread_create(&car_threads[i], NULL, car_thread, &cars[i]);
    }

    // Czekanie na zakończenie wątków samochodów
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_join(car_threads[i], NULL);
    }

    // Czyszczenie mutexów i zmiennych warunkowych
    for (int i = 0; i < NUM_STATIONS; i++) {
        pthread_mutex_destroy(&stations_mutex[i]);
        pthread_cond_destroy(&stations_cond[i]);
    }

    printf("Wszystkie samochody zakończyły jazdę.\n");
    return 0;
}
