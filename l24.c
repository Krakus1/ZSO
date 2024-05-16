#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#define test_errno(msg) do{if (errno) {perror(msg); exit(EXIT_FAILURE);}} while(0)

typedef struct _Arg {        // struktura argumentów dla wątku 1.
        char   napis[256];
        int    rok;
        int    mies;
        int    dzien;
} Argument;
 
void* watek1(void* _arg) {
     Argument* arg = (Argument*)_arg;
 
        printf("Witaj %s w dniu %04d-%02d-%02d",
                arg->napis,
                arg->rok,
                arg->mies,
                arg->dzien
        );
 
        return NULL;
}

void* watek2(void* liczba) {
        char* napis;
        int i;
        printf("Wątek 2 wywołany z argumentem liczbowym %d", (int)liczba); 
}

int main() {
        pthread_t w1, w2;
        Argument arg;

        /* przygotowanie argumentów */
        strcpy(arg.napis, "Wikibooks");
        arg.rok   = 2014;
        arg.mies  = 10;
        arg.dzien = 1;
 
        /* utworzenie dwóch wątków */
        pthread_create(&w1, NULL, watek1, &arg);
        pthread_create(&w2, NULL, watek2, (void*)27);		
}