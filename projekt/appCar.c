#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_CARS 40
#define NUM_STATIONS 2
#define MAX_REFUEL 3
#define MAX_DISTANCE 5
#define FUEL_THRESHOLD 30
#define FUEL_CONSUM 3
#define OIL_CONSUM 2
#define COOLANT_CONSUM 1

// Makro do kontrolowania wyświetlania komunikatów debugujących
#define DEBUG 1

#if DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

pthread_mutex_t stations_mutex[NUM_STATIONS];
pthread_cond_t stations_cond[NUM_STATIONS];
int station_available[NUM_STATIONS];
pthread_mutex_t helper_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t helper_cond = PTHREAD_COND_INITIALIZER;

typedef struct {
    int car_id;
    int fuel;
    int oil;
    int coolant;
    int refuels;
    int is_waiting;
    int distance_to_station;
    int moving_towards_station;
} car_t;

car_t *waiting_car = NULL;

// Funkcja zmieniająca odległość samochodu od stacji
void update_distance_to_station(car_t *car) {
    if (car->distance_to_station >= MAX_DISTANCE) {
        car->moving_towards_station = 1;
    } else if (car->distance_to_station <= 0) {
        car->moving_towards_station = 0;
    }

    if (car->moving_towards_station) {
        car->distance_to_station--;
        LOG("Samochód %d: Przybliżam się do stacji, odległość: %d, paliwo: %d, olej: %d, płyn: %d\n", car->car_id, car->distance_to_station, car->fuel, car->oil, car->coolant);
    } else {
        car->distance_to_station++;
        LOG("Samochód %d: Oddalam się od stacji, odległość: %d, paliwo: %d, olej: %d, płyn: %d\n", car->car_id, car->distance_to_station, car->fuel, car->oil, car->coolant);
    }
}

// Funkcja zużywająca płyny w samochodzie
void consume_fluids(car_t *car) {
    car->fuel -= FUEL_CONSUM * car->car_id;
    car->oil -= OIL_CONSUM * car->car_id;
    car->coolant -= COOLANT_CONSUM * car->car_id;

    // Zapewnienie, że płyny nie są ujemne
    if (car->fuel < 0) car->fuel = 0;
    if (car->oil < 0) car->oil = 0;
    if (car->coolant < 0) car->coolant = 0;
}

// Funkcja uzupełniająca płyny na stacji
void refuel_at_station(car_t *car, int station_id) {
    pthread_mutex_lock(&stations_mutex[station_id]);

    while (!station_available[station_id]) {
        pthread_cond_wait(&stations_cond[station_id], &stations_mutex[station_id]);
    }

    // Zajmowanie stacji
    station_available[station_id] = 0;
    LOG("Samochód %d: Korzystam ze stacji %d\n", car->car_id, station_id);

    // Uzupełnienie jednego płynu
    if (car->fuel <= car->oil && car->fuel <= car->coolant) {
        car->fuel = 100;
        LOG("Samochód %d: Uzupełniono paliwo na stacji %d\n", car->car_id, station_id);
    } else if (car->oil <= car->fuel && car->oil <= car->coolant) {
        car->oil = 100;
        LOG("Samochód %d: Uzupełniono olej na stacji %d\n", car->car_id, station_id);
    } else if (car->coolant <= car->fuel && car->coolant <= car->oil) {
        car->coolant = 100;
        LOG("Samochód %d: Uzupełniono płyn chłodzący na stacji %d\n", car->car_id, station_id);
    }

    car->refuels++;

    LOG("Samochód %d: Zatankowałem na stacji %d, refuels: %d\n", car->car_id, station_id, car->refuels);

    // Zwolnienie stacji
    station_available[station_id] = 1;
    pthread_cond_signal(&stations_cond[station_id]);
    pthread_mutex_unlock(&stations_mutex[station_id]);
}

// Funkcja sprawdzająca, czy potrzebna jest pomoc
void check_and_request_help(car_t *car) {
    if ((car->fuel <= 0 || car->oil <= 0 || car->coolant <= 0) && car->distance_to_station > 0) {
        pthread_mutex_lock(&helper_mutex);
        waiting_car = car;
        car->is_waiting = 1;
        LOG("Samochód %d: Awaria, czekam na pomoc\n", car->car_id);
        pthread_cond_signal(&helper_cond);
        pthread_cond_wait(&helper_cond, &helper_mutex);
        car->is_waiting = 0;
        pthread_mutex_unlock(&helper_mutex);

        car->refuels++;
        LOG("Samochód %d: Naprawiony przez pomoc, refuels: %d\n", car->car_id, car->refuels);
    }
}

void* car_thread(void *arg) {
    car_t *car = (car_t *)arg;

    LOG("Samochód %d: Rozpoczynam jazdę\n", car->car_id);

    while (car->refuels < MAX_REFUEL) {
        sleep(1);

        // Zużycie płynów
        consume_fluids(car);

        // Aktualizacja odległości do stacji
        update_distance_to_station(car);

        // Sprawdzanie, czy potrzebna jest pomoc
        check_and_request_help(car);

        // Przybliżanie się do stacji, gdy paliwo jest niskie i samochód jest w zasięgu stacji
        if (car->fuel <= FUEL_THRESHOLD && car->distance_to_station <= 0) {
            // Szukanie dostępnej stacji
            for (int i = 0; i < NUM_STATIONS; i++) {
                refuel_at_station(car, i);
                if (car->fuel == 100 || car->oil == 100 || car->coolant == 100) {
                    break;
                }
            }
        }
    }

    LOG("Samochód %d: Zakończyłem jazdę\n", car->car_id);
    return NULL;
}

void* helper_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&helper_mutex);
        while (waiting_car == NULL) {
            pthread_cond_wait(&helper_cond, &helper_mutex);
        }

        car_t *car = waiting_car;
        LOG("Samochód pomocy: Otrzymałem zgłoszenie od samochodu %d\n", car->car_id);
        sleep(2);

        // Uzupełnienie płynów
        if (car->fuel == 0) {
            car->fuel = car->distance_to_station * FUEL_CONSUM;
            LOG("Samochód pomocy: Uzupełniono paliwo samochodu %d\n", car->car_id);
        }
        if (car->oil == 0) {
            car->oil = car->distance_to_station * OIL_CONSUM;
            LOG("Samochód pomocy: Uzupełniono olej samochodu %d\n", car->car_id);
        }
        if (car->coolant == 0) {
            car->coolant = car->distance_to_station * COOLANT_CONSUM;
            LOG("Samochód pomocy: Uzupełniono płyn chłodzący samochodu %d\n", car->car_id);
        }

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
        station_available[i] = 1;  // Stacja początkowo jest wolna
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
        cars[i].distance_to_station = 0;
        cars[i].moving_towards_station = 0;

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
