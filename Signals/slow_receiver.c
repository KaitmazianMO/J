#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define ERROR( ... ) { fprintf (stderr, __VA_ARGS__ ); fprintf (stderr, "\n");  exit (-1); }

void set_actions();
void init_globals (const char *out_file);

unsigned char byte = 0;
int           nbit = 0;
int           fd   = 0;
volatile sig_atomic_t         send_pid = 777;
sigset_t      set;

int main (int argc, const char *argv[]) {
    printf ("Receiver pid = %d\n", getpid());

    if (argc != 2) {
        ERROR ("Usage: %s <output-file>", argv[0]);
    }

    init_globals (argv[1]);
    set_actions();

    if (send_pid == 0) {
        ERROR ("UNKNOWN SENDER PID");
    }

    while (1) {
        sigsuspend(&set);
        if (nbit == 8) {
            write (fd, &byte, 1);
            nbit = 0;
            byte = 0;
        }
    }
}

void set_send_pid (int signo, siginfo_t * info, void *i_am_void) {
    send_pid = info->si_pid;
}

void init_globals (const char *out_file) {
    byte = 0;
    nbit = 0;
    fd   = 0;
    send_pid = 0;

    fd = open (out_file, O_CREAT | O_WRONLY, 0660);
    if (fd == -1) {
        ERROR ("%s opening failed", out_file);
    }

    static struct sigaction act_set_send_pid = {};      
    act_set_send_pid.sa_sigaction = set_send_pid;
    act_set_send_pid.sa_flags = SA_SIGINFO;
    sigfillset (&act_set_send_pid.sa_mask);
    if (sigaction (SIGUSR1, &act_set_send_pid, NULL) == -1) {
        ERROR ("act_set_send_pid sigaction error");
    }

    printf ("pause:Receiver pid = %d\n", getpid());
    pause(); // please be a SIGUSR1
    printf ("Sender pid = %d\n", send_pid);

    if (kill (send_pid, SIGUSR1) == -1)
        ERROR ("PIzda");
}

void exit_successfully (int signo) {
    exit (0);
}

void one (int signo) {
    byte |= (0x1 << nbit++);
    kill (send_pid, SIGUSR1);
}

// SIGUSR2
void zero (int signo) { 
    byte &= ~(0x1 << nbit++);
    kill (send_pid, SIGUSR1);
}

void set_actions() {
    struct sigaction act_term;
    memset (&act_term, 0, sizeof (act_term));
    act_term.sa_handler = exit_successfully; 
    sigfillset (&act_term.sa_mask); 
    sigaction (SIGTERM, &act_term, NULL);

    struct sigaction act_one;
    memset (&act_one, 0, sizeof (act_one));
    act_one.sa_handler = one;
    sigfillset (&act_one.sa_mask);
    sigaction (SIGUSR1, &act_one, NULL);
  
    struct sigaction act_zero;
    memset (&act_zero, 0, sizeof (act_zero));
    act_zero.sa_handler = zero;
    sigfillset (&act_zero.sa_mask);  
    sigaction (SIGUSR2, &act_zero, NULL);  
  
    sigemptyset (&set);
    sigaddset (&set, SIGUSR1);
    sigaddset (&set, SIGUSR2);
    sigaddset (&set, SIGTERM);
    sigprocmask (SIG_BLOCK, &set, NULL);

    sigemptyset (&set);
}
