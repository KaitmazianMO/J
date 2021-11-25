#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct msgp{
    long mtype;
    char buff[1024];
} msgp;

int fatal (const char *msg) {
    fprintf (stderr, "FATAL: %s failed\n", msg);
    exit (1);
}

int main() {
    const key_t key = ftok ("msgkey", 1);
    if (key == -1) {
        fatal ("ftok");
    }
    const int msg_id = msgget (key, 0666 | IPC_CREAT);
    if (msg_id == -1) {
        fatal ("msgget");
    }

    while (1) {
        msgp msg_buff = { .mtype = 1, .buff = {} };
        fgets (msg_buff.buff, (int)sizeof msg_buff.buff, stdin);
        if (msgsnd (msg_id, &msg_buff, sizeof msg_buff, 0) == -1) {
            fatal ("senging failed");
        }
    }   
}