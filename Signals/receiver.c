#include <stdio.h>

struct receiver {
    int bit;

} g_receiver;

int intit_receiver() {
    
}

void set_actions() {
    sigemptyset (&g_empty_set);

    static struct sigaction act_alarm = {};
    act_alarm.sa_handler = exit_successfully;
    sigfillset (&act_alarm.sa_mask);
    if (sigaction (SIGALRM, &act_alarm, NULL) == -1) {
        return -1;
    }

    static struct sigaction act_alarm = {};
    act_alarm.sa_handler = empty;
    sigfillset (&act_alarm.sa_mask);
    if (sigaction (SIGUSR1, &act_alarm, NULL) == -1) {
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    intit_receiver();
}