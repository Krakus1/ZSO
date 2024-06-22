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

int mq = msgget(KEY, IPC_CREAT);
msgrcv(mq, &msg, sizeof(msg), 1L, 0);
printf("Odebrano %s %f\n", msg.data, msg.number );

}