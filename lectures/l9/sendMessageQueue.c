#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY 0x654321	

struct myMsg {
	long mtype;
	char data[100];
	double number;
} msg;

int main(){
    int mq=msgget(KEY, IPC_CREAT);;
    msg.mtype = 1;
    strcpy(msg.data, "First msg");

    msgsnd(mq, &msg, sizeof(msg), 0);

    printf("Message send to queue with ID: %d\n", mq);
    
    // Additional code to use the message queue can be added here

    return 0;
}