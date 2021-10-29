#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "parser.h"

long file_size (FILE *file);
char *read_file (FILE *file);

int fatal (const char *msg);
#define ERROR_HANDLING_CALL( func_call )    \
    if (func_call == -1) { \
        fatal (#func_call); \
    }


#define PIPE_READ  (0)
#define PIPE_WRITE (1)

int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf ("Usage: %s <file>\n", argv[0]);
        return 9;
    }

    FILE *cmd_file = fopen (argv[1], "rb");
    if (!cmd_file) {
        printf ("Can't open a file %s\n", argv[1]);
        return 109;
    }

    char *command_line = read_file (cmd_file);
    fclose (cmd_file);    
    if (!command_line) {
        printf ("File reading failed\n");
        return 202;
    }
    
    Parser parser = {};

    if (parse (&parser, command_line) == 0) {
        int *pipes = calloc ((parser.ncmds - 1)*2, sizeof (pipes[0]));
        if (!pipes) {
            printf ("File reading failed\n");
            free (command_line);
            fclose (cmd_file);
            return 258;
        }

        for (int i = 0; i < parser.ncmds - 1; ++i) {
            ERROR_HANDLING_CALL (pipe (pipes + i*2));
        }

        int pipe_read       = PIPE_READ;
        int pipe_write      = PIPE_WRITE;
        for (size_t i = 0; i < parser.ncmds - 1; 
            ++i,
            pipe_read       += 2,
            pipe_write      += 2) {
            switch (fork()) {
                case -1: {
                    fatal ("fork failed");
                }
                case 0: { /* child */
                    ERROR_HANDLING_CALL (close (pipes[pipe_read]));
                    ERROR_HANDLING_CALL (dup2 (pipes[pipe_write], STDOUT_FILENO));
                    ERROR_HANDLING_CALL (execvp (parser.cmds[i].argv[0], parser.cmds[i].argv));
                    //ERROR_HANDLING_CALL (execlp ("ls", "ls", "-la", "/", NULL));
                }
                default: { /* parent */
                    ERROR_HANDLING_CALL (wait (NULL));
                    break;
                }
            }

        }
        free (pipes);
    } else {
        printf ("parsing failed\n");
    }

    free (command_line);

    return 0;
}

int fatal (const char *msg) {
    fprintf (stderr, "FATAL: %s failed\n", msg);
    fprintf (stderr, "%s\n", strerror (errno));    
    exit (1);
}

char *read_file (FILE *file) {
    if (!file) return NULL;

    const long fsize = file_size (file);
    if (fsize <= 0) {
        return NULL;
    }

    char *buff = (char *)calloc (fsize + 1, sizeof (char));
    if (buff) {
        buff[fsize] = '\0';
        if (fread (buff, sizeof(char), fsize, file) != fsize) {
            free (buff);
            buff = NULL;
        }
    }

    return buff;
}

long file_size (FILE *file) {
    if (!file) return -1;

    struct stat st = {};
    if (fstat (fileno (file), &st) == -1) {
        return -1;
    }

    return st.st_size;
}