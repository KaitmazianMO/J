#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
    char **argv;
    int argc;
} Command;

typedef struct {
    Command *cmds;
    int ncmds;
} Parser;

static int count_words (const char *str);
static int count_char (const char *str, char ch);

static Command parse_command(const char *cmdstr);

int parse (Parser *parser, char *line) {
    if (!(parser && line)) return -1;
    parser->ncmds = count_char (line, '|') + 1;
    parser->cmds = calloc (parser->ncmds, sizeof (parser->cmds[0]));
    if (!parser->cmds) return -1;
    
    line = strtok (line , "|");    
    for (int i = 0; i < parser->ncmds; ++i, line = strtok (NULL, "|")) {
        parser->cmds[i] = parse_command (line);
        if (parser->cmds[i].argv == NULL) {
            free (parser->cmds);
            return -1;
        }
    }
    
    return 0;
}

Command parse_command(const char *cmdstr) {
    Command cmd = {};
    cmd.argc = count_words (cmdstr);
    cmd.argv = calloc (cmd.argc, sizeof (cmd.argv[0]));
    if (cmd.argv) {
        for (int i = 0; i < cmd.argc; ++i) {
            while (isspace (*cmdstr)) cmdstr++;
            if (sscanf (cmdstr, "%ms", cmd.argv + i) > 0) {
                cmdstr += strlen (cmd.argv[i]);
            } else {
                for (int j = 0; j < i; ++j) {
                    free (cmd.argv[i]);
                }
                free (cmd.argv);
                cmd.argv = NULL;
                break;
            }
        }
    }

    return cmd;
}

static int count_words(const char *str) {
    int count = isspace(str[0]) ? 0 : 1;
    for (int i = 0; str[i+1]; ++i) {
           if (isspace(str[i]) && !isspace(str[i+1])) {
            ++count;
        }
    }
    return count;
}

int count_char (const char *str, char ch) {
    const size_t len = strlen (str); 
    int count = 0;
    for (size_t i = 0; i < len; ++i) {
        if (str[i] == ch) {
            ++count; 
        }
    }

    return count;
}

int main() {
    Parser parser = {};
    char command_line[] = " \t ls -la //|\t grep bin | test --flag tou\tfag xd ";
    
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

    return 0;
}
