#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

pthread_t tid[600];
pthread_barrier_t bariera;

void * thread_function(void * arg) {
	pthread_barrier_wait (&bariera);
	sleep(1); // czas oczekiwania 1 sekunda
	return 0;
}

int main() {
	pthread_barrier_init (&bariera, NULL, 300);
	
	for(int i=0; i < 600; i++)
		pthread_create (&tid[i], NULL, thread_function, NULL);

	for(int i=0; i < 600; i++)
		pthread_join (tid[i], NULL); 

	pthread_barrier_destroy (&bariera);
}