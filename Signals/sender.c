#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SZ  (1 << 20)
static uint8_t chunk[CHUNK_SZ];

#define ONE_SIGNAL   (SIGUSR1)
#define ZERO_SIGNAL  (SIGUSR2)

#define ERROR( ... ) { fprintf (stderr, __VA_ARGS__ ); fprintf (stderr, "\n"); kill (g_sender.rec_pid, SIGTERM); return -1;}

int init_sender(const char *file_to_send, const char *rec_pid_str, uint8_t *buff, size_t buff_cap);
int free_sender();
int read_chunk();
int send_file();
int send_chunk();
int send_byte(size_t nbyte);

struct Sender {
    int      fd;
    int      rec_pid;
    uint8_t *chunk;
    size_t   chunk_cap;
    size_t   chunk_sz;
} g_sender;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        ERROR("Usage: %s <file-to-send> <receiver-pid>", argv[0]);
    }

    int ret = 0;
    if (init_sender(argv[1], argv[2], chunk, CHUNK_SZ)) {
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

int send_file() {
    int read = 0;
    while (read = read_chunk()) {
        if (read == -1) {
            fpritnf (stderr, "Chunk reading failed");
            return 1;
        }

        send_chunk();
    }
    kill (g_sender.rec_pid, SIGTERM);
    return 0;
}

int send_chunk() {
    int err = 0;
    for (size_t i = 0, size = g_sender.chunk_sz; i < size; ++i) {
        send_byte(i);
    }
    return 0;
}

int send_byte(size_t nbyte) {
    uint8_t byte = g_sender.chunk[nbyte];
    int pid = g_sender.rec_pid;
    for (size_t i = 0; i < 8; ++i) {
        if ((byte << i) & 0x1) {
            kill(pid, ONE_SIGNAL);
        } else {
            kill(pid, ZERO_SIGNAL);
        }  
    }
}

int read_chunk() {
    return g_sender.chunk_sz = (g_sender.fd, g_sender.chunk, g_sender.chunk_cap);
}

int free_sender() {
    return close (g_sender.fd);
}