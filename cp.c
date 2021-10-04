#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFF_SZ 256

int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf (stderr, "Usage: %s <file-to-copy> <file-to-writing>", argv[0]);
        exit (1);
    }

    if (strcmp (argv[1], argv[2]) == 0)
    {
        fprintf (stderr, "nope\n");
        exit (1);    
    }

    int input_d  = open (argv[1], O_RDONLY);
    if (input_d == -1)
    {
        fprintf (stderr, "File open error(%s).", argv[1]);
        exit (errno);
    }

    int output_d = open (argv[2], O_WRONLY | O_CREAT, 0777);
    if (output_d == -1)
    {
        fprintf (stderr, "File open error(%s).", argv[2]);
        exit (errno);
    }

    char buff[BUFF_SZ + 1] = {};    
    long nread = 0;
    while ((nread = read (input_d, buff, BUFF_SZ)) > 0)
    {
        buff[nread] = '\0';
        if (write (output_d, buff, nread) <= 0)
        {
            printf ("Writting error.");
            exit (errno);
        }
    }

    if (nread == -1)
    {
            printf ("Reading error.");
            exit (errno);  
    }

    return errno;
}
