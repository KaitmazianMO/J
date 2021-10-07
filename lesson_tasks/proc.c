#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define N_PROC 1024

int main (int argc, char *argv[])
{
    for (int i = 0; i < N_PROC; ++i)
    {
        exec (fork());
    }

    return 0;
}