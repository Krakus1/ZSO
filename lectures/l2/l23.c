#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#define test_errno(msg) do{if (errno) {perror(msg); exit(EXIT_FAILURE);}} while(0)


void* watek(void* arg) {
        sleep(1000);
        return NULL;
}

int main() {
	pthread_t id1, id2, id3;
 
	pthread_create(&id1, NULL, watek, NULL);
	pthread_create(&id2, NULL, watek, NULL);
	pthread_create(&id3, NULL, watek, NULL); 

	pthread_exit(NULL);
	return EXIT_SUCCESS;
}