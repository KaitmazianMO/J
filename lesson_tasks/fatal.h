#ifndef FATAL_H_INCLUDED
#define FATAL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

int fatal (const char *msg) {
    fprintf (stderr, "FATAL: %s failed\n", msg);
    exit (1);
}

#endif // FATAL_H_INCLUDED