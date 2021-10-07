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

    FILE *fout = fopen (argv[1], "wb");
    assert (fout);

    for (int i = 0; i < 10; ++i)
    {
        fork();
        wait (0);
    }

    fprintf (fout, "pid: %4i parent: %4i\n", getpid(), getppid());

    return 0;
}