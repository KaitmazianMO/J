#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define ERROR( ... ) { fprintf (stderr, __VA_ARGS__ ); fprintf (stderr, "\n"); kill (rec_pid, SIGTERM); exit (-1);}

unsigned char byte = '\0';
pid_t         rec_pid = 0;
int           fd = -1;
sigset_t      set;

void init_globals (const char *file_name, const char *pid_str);
void set_actions();

int main(int argc, char *argv[]) {
    printf ("send pid = %d\n", getpid());

    if (argc != 3) {
        ERROR("Usage: %s <file-to-send> <receiver-pid>", argv[0]);
    }

    init_globals (argv[1], argv[2]);
    set_actions();

    kill (rec_pid, SIGUSR1); // start sending

    alarm (5);

    sigsuspend (&set);

    int nread = 0;
    while ((nread = read (fd, &byte, 1))) {
        if (nread == -1) {
            ERROR ("Reading error");
        }

        alarm (5);

        for (int i = 0; i < 8; ++i) {
            if ((byte >> i) & 0x1) {
                kill (rec_pid, SIGUSR1);
            } else {
                kill (rec_pid, SIGUSR2);
            }
            sigsuspend (&set);
        }
    }

    kill (rec_pid, SIGTERM);
    return 0;
}


void init_globals (const char *file_name, const char *pid_str) {
    fd = open (file_name, O_RDONLY);
    if (fd == -1) {
        ERROR("%s opening failed", file_name);
    }    

    char *end = NULL;
    rec_pid = strtol(pid_str, &end, 10);
    if (end - pid_str != strlen (pid_str) || errno != 0) {
        ERROR ("Unconvertable pid(%s)", pid_str);
    }
}

void exit_unsuccessfully (int signo) {
    exit (-1);
}

void empty (int signo) {
    ;
}

void set_actions() {
    static struct sigaction act_alarm;      
    memset(&act_alarm, 0, sizeof(act_alarm));
    act_alarm.sa_handler = exit_unsuccessfully;
    sigfillset (&act_alarm.sa_mask);
    if (sigaction (SIGALRM, &act_alarm, NULL) == -1) {
        ERROR ("alarm sigaction error");
    }

    static struct sigaction act_empty;                    
    memset(&act_empty, 0, sizeof(act_empty));
    act_empty.sa_handler = empty;
    sigfillset(&act_empty.sa_mask);    
    if (sigaction(SIGUSR1, &act_empty, NULL) == -1) {
        ERROR ("sigaction error");
    }

    sigemptyset (&set);
    sigaddset (&set, SIGUSR1);
    sigaddset (&set, SIGUSR2);
    sigaddset (&set, SIGTERM);
    sigprocmask (SIG_BLOCK, &set, NULL);

    sigemptyset (&set);
}
