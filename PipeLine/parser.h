#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "command.h"

typedef struct {
    Command *cmds;
    int ncmds;
} Parser;

extern int parse (Parser *parser, char *line);
extern void free_parser (Parser *parser);

#endif // PARSER_H_INCLUDED