#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "parser.h"

long file_size (FILE *file);
char *read_file (FILE *file);


int fatal (const char *msg);
#define ERROR_HANDLING_CALL( func_call )    \
    if (func_call == -1) { \
        fatal (#func_call); \
    }


int main(int argc, char *argv[]) {
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
    if (!command_line) {
        printf ("File reading failed\n");
        fclose (cmd_file);
        return 202;
    }
    printf ("read %s\n", command_line);
    
    Parser parser = {};
    if (parse (&parser, command_line) == 0) {
        for (int i = 0; i < parser.ncmds; ++i) {
            for (int j = 0; j < parser.cmds[i].argc; ++j) {
                printf ("\'%s\' ", parser.cmds[i].argv[j]);
            }
            printf ("\n");
        }
    } else {
        printf ("parse failed\n");
    }

    //for (size_t i = 0; i < nproc - 1; ++i) {
    //switch (fork()) {
    //    case -1: {
    //        fatal ("fork failed");
    //    }
    //    case 0: { /* child */
    //        ERROR_HANDLING_CALL (close (pipeline1[PIPE_READ]));
    //        ERROR_HANDLING_CALL (dup2 (pipeline1[PIPE_WRITE], STDOUT_FILENO));
    //        ERROR_HANDLING_CALL (execvp (processes[i][0], processes[i]));
    //        //ERROR_HANDLING_CALL (execlp ("ls", "ls", "-la", "/", NULL));
    //    }
    //    default: { /* parent */
    //        ERROR_HANDLING_CALL (wait (&ret_code));
    //        break;
    //    }
    //}
    //switch (fork()) {
    //    case -1: {
    //        fatal ("fork failed");
    //    }
    //    case 0: { /* child */
    //        ERROR_HANDLING_CALL (close (pipeline1[PIPE_WRITE]));
    //        ERROR_HANDLING_CALL (dup2 (pipeline1[PIPE_READ], STDIN_FILENO));
    //        //ERROR_HANDLING_CALL (close (pipeline1[PIPE_READ]));
    //        //ERROR_HANDLING_CALL (dup2 (pipeline2[PIPE_WRITE], STDOUT_FILENO));
    //        ERROR_HANDLING_CALL (execvp (processes[i + 1][0], processes[i + 1]));
    //    }
    //    default: { /* parent */
    //        ERROR_HANDLING_CALL (close (pipeline1[PIPE_READ]));
    //        ERROR_HANDLING_CALL (close (pipeline1[PIPE_WRITE]));
    //        ERROR_HANDLING_CALL (wait (&ret_code));
    //        break;
    //    }        
    //}    

    return 0;
}

long file_size (FILE *file) {
    if (!file) return -1;

    struct stat st = {};
    if (fstat (fileno (file), &st) == -1) {
        return -1;
    }

    return st.st_size;
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

int fatal (const char *msg) {
    fprintf (stderr, "FATAL: %s failed\n", msg);
    exit (1);
}