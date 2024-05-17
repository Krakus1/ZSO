#include <stdio.h>
#include <pthread.h>
#define STEPS 10

pthread_t reader1, reader2, reader3, writer;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int library[STEPS / 2];
int position = 0;
int reading_action_number = 0;

void *read_thread(void *arg) {
        int book = 1;
        int reader_number = (int)arg;
        usleep(10);
        while (book > 0) {
                usleep(10);
                pthread_rwlock_rdlock(&rwlock);
                if(position > 0) {
                        book = library[position - 1];
                        printf("reader=%d reading_action_number=%d book=%d", reader_number, reading_action_number, book);
                        reading_action_number++;
                        fflush(stdout);
                }
                pthread_rwlock_unlock(&rwlock);
        }
        return 0;
}

void *writer_thread(void *arg) {
        int book = STEPS / 2;
        while (book >= 0) {
                usleep(10);
                pthread_rwlock_wrlock(&rwlock);
                library[position] = book;
                position++;
                pthread_rwlock_unlock(&rwlock);
                book--;
        }
        return 0;
}


int main() {
        pthread_rwlock_init(&rwlock, NULL);

        pthread_create(&reader1, NULL, read_thread, (void*)1);
        pthread_create(&reader2, NULL, read_thread, (void*)2);
        pthread_create(&reader3, NULL, read_thread, (void*)3);
        pthread_create(&writer, NULL, writer_thread, NULL);

        pthread_join(reader1, NULL);
        pthread_join(reader2, NULL);
        pthread_join(reader3, NULL);
        pthread_join(writer, NULL);

        pthread_rwlock_destroy(&rwlock);
}
	