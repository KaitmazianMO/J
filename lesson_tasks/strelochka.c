#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>

int main (int argc, char *argv[])
{
    int fd_out = open (argv[1], O_WRONLY | O_CREAT | O_TRUNC);
    if (fd_out == -1) {
        fprintf (stderr, "Can't open %s\n", argv[1]);
        return 1;
    }
    
    dup2 (fd_out, STDOUT_FILENO);
    if (execvp (argv[2], &argv[2]) == -1) {
        fprintf (stderr, "Can't open %s\n", argv[1]);
        return 1;
    }

    //close (fd_out);
    return 0;
}