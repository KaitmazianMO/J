#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include "fatal.h"

int read_lock (int fd);

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fatal ("Look at the desk");
    }

    const char *file_name = argv[1];
    
    int fd = open (file_name, O_RDONLY);

    if (read_lock (fd) == -1) {
        close (fd);
        fatal ("Lock reading failed");
    }

    close (fd);
    return 0;
}

int read_lock (int fd) {
    struct flock fl = {};   

    if (fcntl (fd, F_GETLK, &fl) == -1) {
        return -1;
    }

    if ((fl.l_type & F_WRLCK)) {
        printf ("Write lock\n");
    }

    if (fl.l_type) {
        printf ("start: %ld\nlen: %ld\n", fl.l_start, fl.l_len);
    }

    return 0;
}

int set_lock (int fd, long pos, long size) {
    struct flock fl = {
        .l_type     = F_RDLCK | F_WRLCK,
        .l_whence   = SEEK_SET,
        .l_start    = pos,
        .l_len      = size,
        .l_pid      = getpid()
    };   

    return fcntl (fd, F_SETLK, &fl);
}