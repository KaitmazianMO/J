#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR_HANDLING_CALL( func_call )    \
    if (func_call == -1) { \
        fatal (#func_call); \
    }

int fatal (const char *msg);

#define PIPE_READ  (0)
#define PIPE_WRITE (1)

int main (int argc, char *argv[]) {
    int pipeline1[2] = {};
    int pipeline2[2] = {};
    char *const processes[][256] = {
        { "ls",   "-la", "/", NULL },
        { "grep", "bin",      NULL },
    };

    ERROR_HANDLING_CALL (pipe (pipeline1));
    ERROR_HANDLING_CALL (pipe (pipeline2));

    size_t nproc = sizeof (processes) / 256 / 8;
    int ret_code = 0;
    for (size_t i = 0; i < nproc - 1; ++i) {
        switch (fork()) {
            case -1: {
                fatal ("fork failed");
            }
            case 0: { /* child */
                ERROR_HANDLING_CALL (close (pipeline1[PIPE_READ]));
                ERROR_HANDLING_CALL (dup2 (pipeline1[PIPE_WRITE], STDOUT_FILENO));
                ERROR_HANDLING_CALL (execvp (processes[i][0], processes[i]));
                //ERROR_HANDLING_CALL (execlp ("ls", "ls", "-la", "/", NULL));
            }
            default: { /* parent */
                ERROR_HANDLING_CALL (wait (&ret_code));
                printf ("%s returns %d\n", processes[i][0], ret_code);  
                break;
            }
        }

        switch (fork()) {
            case -1: {
                fatal ("fork failed");
            }
            case 0: { /* child */
                ERROR_HANDLING_CALL (close (pipeline1[PIPE_WRITE]));
                ERROR_HANDLING_CALL (dup2 (pipeline1[PIPE_READ], STDIN_FILENO));
                //ERROR_HANDLING_CALL (close (pipeline1[PIPE_READ]));
                //ERROR_HANDLING_CALL (dup2 (pipeline2[PIPE_WRITE], STDOUT_FILENO));
                ERROR_HANDLING_CALL (execvp (processes[i + 1][0], processes[i + 1]));
            }
            default: { /* parent */
                ERROR_HANDLING_CALL (close (pipeline1[PIPE_READ]));
                ERROR_HANDLING_CALL (close (pipeline1[PIPE_WRITE]));
                ERROR_HANDLING_CALL (wait (&ret_code));
                printf ("%s returns %d\n", processes[i + 1][0], ret_code);  
                break;
            }        
        }    
    }
    return 0;
}

int fatal (const char *msg)
{
    fprintf (stderr, "FATAL: %s failed\n", msg);
    exit (1);
}