#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Struktura argumentów dla wątku
typedef struct {
    int thread_num;
    int sleep_time;
    int *child_threads;
    int *child_sleep_times;
    int num_children;
} thread_arg_t;

typedef struct 
{
    int numCar;
    int fuel;
    int countCars;
} carParam;

void refuel()
{

}

void *car(void *arg)
{
    carParam *tcarParam = (carParam *) arg;
    int numCar = tcarParam->numCar;
    int fuels[] = tcarParam->fuel;
    int countCars = tcarParam->countCars;

    printf("Wątek %d: Żyję\n", numCar);
    fflush(stdout);


    if (countCars > 0)
    {
        pthread_t *cars = malloc(countCars * sizeof(pthread_t));
        carParam *tcarParam = malloc(countCars * sizeof(carParam));

        for(int i = 0; i < countCars; i++)
        {
            tcarParam[i].fuel = fuels[i];

            pthread_create(&cars[i], NULL, car, &tcarParam[i]);
        }

        for (int i = 0; i < countCars; i++) {
            int *end_time;
            pthread_join(cars[i], (void **) &end_time);
            free(end_time);
        }

        free(cars);
        free(tcarParam);        
    }


}

void create_cars(int thread_num, int fuel[],int count_cars)
{
    pthread_t thread;

    carParam param = {fuel, 6};
    pthread_create(&thread,NULL,car, &param);
    pthread_join(thread, NULL);
}

int main()
{
    int fuel[] = {10,11,12,13,14,15};
    create_cars(1,fuel,6);

//watek auta
//watek stacja_benz

// int thread1_children[] = {3, 4, 5, 6, 7, 8, 9, 10};
// int thread1_sleep_times[] = {2, 2, 2};

// //utworzenie samochodow
// create_threads(1, 2, thread1_children,thread1_sleep_times, 10);
}