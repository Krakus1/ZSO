#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY 0x654321

int main() {
    int mq = msgget(KEY, IPC_CREAT);

    msgctl(mq, IPC_RMID, NULL);

    printf("Message queue delete with ID: %d\n", mq);
    
    return 0;
}