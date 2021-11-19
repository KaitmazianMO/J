#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SZ  (1 << 20)
static uint8_t g_chunk[CHUNK_SZ];

#define ONE_SIGNAL   (SIGUSR1)
#define ZERO_SIGNAL  (SIGUSR2)

#define ERROR( ... ) { fprintf (stderr, __VA_ARGS__ ); fprintf (stderr, "\n"); kill (g_sender.rec_pid, SIGTERM); return -1;}

int init_sender(const char *file_to_send, const char *rec_pid_str, uint8_t *buff, size_t buff_cap);
int free_sender();
int read_chunk();
int send_file();
int send_chunk();
int set_actions();
int send_byte(size_t nbyte);

struct Sender {
    int      fd;
    pid_t    rec_pid;
    uint8_t *chunk;
    size_t   chunk_cap;
    size_t   chunk_sz;
} g_sender;

sigseg_t g_empty_set = {};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        ERROR("Usage: %s <file-to-send> <receiver-pid>", argv[0]);
    }

    int ret = 0;
    if (init_sender(argv[1], argv[2], g_chunk, CHUNK_SZ)) {
        set_actions();
        ret = send_file();
        if (ret == 0) {
            fprintf (stderr, "%s sended successfully", argv[1]);
        } else {
            fprintf (stderr, "%s sending failed", argv[1]);
        }
    }
    free_sender();
    return ret;
}

int init_sender(const char *file_to_send, const char *rec_pid_str, 
                uint8_t *buff, size_t buff_cap) {
    char *end = NULL;
    int rec_pid = strtol(rec_pid_str, &end, 10);
    if (end - rec_pid_str != strlen (rec_pid_str)) {
        ERROR ("Unconvertable pid(%s)", rec_pid_str);
    }

    int fd = open(file_to_send, O_RDONLY);
    if (fd == -1) {
        ERROR("%s opening failed", file_to_send);
    }

    g_sender.chunk     = buff;
    g_sender.chunk_sz  = 0;
    g_sender.chunk_cap = buff_cap;
    g_sender.fd        = fd;
    g_sender.rec_pid   = rec_pid;
    return 0;
}

void exit_successfully (int signo) {
    exit (0);
}

void empty (int signo) {
    ;
}

int set_actions() {
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

int send_file() {
    int read = 0;
    while (read = read_chunk()) {
        if (read == -1) {
            fpritnf (stderr, "Chunk reading failed");
            return 1;
        }

        if (send_chunk() == -1) {
            return -1;
        };
    }
    kill (g_sender.rec_pid, SIGTERM);
    return 0;
}

int send_chunk() {
    int err = 0;
    for (size_t i = 0, size = g_sender.chunk_sz; i < size; ++i) {
        if (send_byte(i) == -1) {
            return -1;
        };
    }
    return 0;
}

int send_byte(size_t nbyte) {
    uint8_t byte = g_sender.chunk[nbyte];
    int pid = g_sender.rec_pid;
    
    sigalarm (1); // alarm for chunks ?

    for (size_t i = 0; i < 8; ++i) {
        if ((byte << i) & 0x1) {
            if (kill(pid, ONE_SIGNAL) == -1) {
                return -1;
            }
        } else {
            if (kill(pid, ZERO_SIGNAL) == -1) {
                return -1;
            }
        }  

    sigsuspend (&g_empty_set);
    }

    return 0;
}

int read_chunk() {
    return g_sender.chunk_sz = (g_sender.fd, g_sender.chunk, g_sender.chunk_cap);
}

int free_sender() {
    return close (g_sender.fd);
}