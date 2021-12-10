#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include "fatal.h"

int set_lock (int fd, long pos, long size);
int set_unlock (int fd, long pos, long size);

int main (int argc, char *argv[]) {
    if (argc != 4) {
        fatal ("Look at the desk");
    }

    errno = 0;
    const char *file_name = argv[1];
    const long  pos       = strtol (argv[2], NULL, 10);
    const long  size      = strtol (argv[3], NULL, 10);
    //printf ("pos = %ld\nsize = %ld\n", pos, size);
    if (errno) {
        fatal ("argument parsing failed");
    }

    if (pos < 0 || size < 0) {
        fatal ("Invalid argiments");
    }

    int fd = open (file_name, O_RDWR);
    if (fd == -1) {
        fatal ("bad file");
    }

    if (set_lock (fd, pos, size) == -1) {
        close (fd);
        fatal ("Locking failed");
    }
    struct timeval tm = { .tv_sec = 3, .tv_usec = 142 };
    select (0, NULL, NULL, NULL, &tm); // :) 
    if (set_unlock (fd, pos, size) == -1) {
        close (fd);
        fatal ("Unlocking failed");
    }

    close (fd);
    return 0;
}

int set_unlock (int fd, long pos, long size) {
    struct flock fl = {
        .l_type     = F_UNLCK,
        .l_whence   = SEEK_SET,
        .l_start    = pos,
        .l_len      = size,
        .l_pid      = getpid()
    };   

    return fcntl (fd, F_SETLK, &fl);
}

int set_lock (int fd, long pos, long size) {
    struct flock fl = {
        .l_type     = F_WRLCK,
        .l_whence   = SEEK_SET,
        .l_start    = pos,
        .l_len      = size,
        .l_pid      = getpid()
    };   

    return fcntl (fd, F_SETLK, &fl);
}