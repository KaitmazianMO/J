#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

typedef struct {
    char **argv;
    int argc;
} Command;

extern void free_cmd (Command *cmd);

#endif