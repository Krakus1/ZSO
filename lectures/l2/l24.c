#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define test_errno(msg) do{if (errno) {perror(msg); exit(EXIT_FAILURE);}} while(0)

typedef struct _Arg {        // struktura argumentów dla wątku 1.
        char   napis[256];
        int    rok;
        int    mies;
        int    dzien;
} Argument;
 
void* watek1(void* _arg) {
        Argument* arg = (Argument*)_arg;
 
        printf("Witaj %s w dniu %04d-%02d-%02d\n",
                arg->napis,
                arg->rok,
                arg->mies,
                arg->dzien
        );
 
        return NULL;
}
void* watek2(void* _arg) {
        Argument* arg = (Argument*)_arg;
 
        arg->rok = 2013;

        return NULL;
}

// void* watek2(void* liczba) {
//         char* napis;
//         int i;
//         printf("Wątek 2 wywołany z argumentem liczbowym %d", (int)liczba); 
// }

int main() {
        pthread_t w1,w2,w3;
        Argument arg_1;

        /* przygotowanie argumentów */
        strcpy(arg_1.napis, "Wikibooks");
        arg_1.rok   = 2014;
        arg_1.mies  = 10;
        arg_1.dzien = 1;

        /* utworzenie dwóch wątków */
 	errno = pthread_create(&w1, NULL, watek1, &arg_1);
        test_errno("Nie powiodło się pthread_create");

        errno = pthread_join(w1, NULL);
        test_errno("pthread_join");        

 	errno = pthread_create(&w2, NULL, watek2, &arg_1);
        test_errno("Nie powiodło się pthread_create");

        errno = pthread_join(w2, NULL);
        test_errno("pthread_join");      

 	errno = pthread_create(&w3, NULL, watek1, &arg_1);
        test_errno("Nie powiodło się pthread_create");

        errno = pthread_join(w3, NULL);
        test_errno("pthread_join");              
        //pthread_create(&w2, NULL, watek2, (void*)27);		
}