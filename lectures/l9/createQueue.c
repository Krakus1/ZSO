#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY 0x654321

struct myMsg {
	long mtype;
	char data[100];
	double number;
} msg;

int main() {
    struct myMsg msg;
    int mq;

    // Initialize the message queue
    mq = msgget(KEY, IPC_CREAT | 0666);
    if (mq == -1) {
        perror("msgget failed");
        return 1;
    }

    printf("Message queue created with ID: %d\n", mq);
    
    // Additional code to use the message queue can be added here

    return 0;
}