#include <stdio.h>
#include <dirent.h>
#include <errno.h>

#include "fatal.h"

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fatal ("Usage: look at the board");
    }

    DIR *dir = opendir (argv[1]);
    if (!dir) {
        fatal ("dir openning failed");
    }

    struct dirent *pdirent;
    while ((pdirent = readdir (dir))) {
        if (pdirent->d_type == DT_DIR) {
            printf ("%s\n", pdirent->d_name);
        }
    }

    if (errno) {
        fatal ("dir reading failed");
    }

    closedir (dir);
    return 0;
}