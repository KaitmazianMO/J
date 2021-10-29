#include "command.h"
#include <stdlib.h>

extern void free_cmd (Command *cmd) {
    for (int i = 0; i < cmd->argc; ++i) {
        free (cmd->argv[i]);
    }
}