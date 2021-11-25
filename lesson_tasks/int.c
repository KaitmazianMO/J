#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

int fatal (const char *msg) {
    fprintf (stderr, "FATAL: %s failed\n", msg);
    exit (1);
}

double func (double x) {
    return x/sin(x);
}

struct section {
    double from, to;
    int n_points;
    double sum;
};

void *compute_part (void *sec_void) {
    struct section *sec = sec_void;
    sec->sum = 0;
    double dx = (sec->to - sec->from)/sec->n_points;
    for (double x = sec->from; x < sec->to; x += dx) {
        sec->sum += func (x)*dx;
    }

    return NULL;
}

int main (int argc, char *argv[]) {
    if (argc != 3) {
        fatal ("Usage: look at the board");
    }

    errno = 0;
    int n_threads = strtol (argv[1], NULL, 10);
    int n_points  = strtol (argv[2], NULL, 10);
    if (errno) {
        fatal ("erryes");
    }   

    printf ("nthreads = %d\n", n_threads);
    printf ("npoints  = %d\n", n_points);

    pthread_t *threads = calloc (n_threads, sizeof (*threads));
    if (!threads) {
        fatal ("mem aloc failed");
    }

    struct section *secs = calloc (n_threads, sizeof (*secs)); 
    if (!secs) {
        fatal ("mem aloc failed");
    }   

    double pi   = 3.14159265;
    double from = pi / 4;
    double to   = pi / 2;
    double dX = (to - from) / n_threads;
    struct section curr_sec = { .from = from, .to = from + dX, .n_points = n_points/n_threads };
    for (int i = 0; i < n_threads; ++i) {
        secs[i] = curr_sec;
        curr_sec.from = curr_sec.to;
        curr_sec.to += dX;
    }

    if (pthread_setconcurrency (n_threads) == -1) {
        fatal ("conc failed");
    }

    for (int i = 0; i < n_threads; ++i) {
        if (pthread_create (threads + i, NULL, compute_part, secs + i) == -1) {
            fatal ("tread creating failed");
        }
    }

    for (int i = 0; i < n_threads; ++i) {
        if (pthread_join (threads[i], NULL) == -1) {
            fatal ("tread join failed");
        }        
    }

    double SUM = 0;
    for (int i = 0; i < n_threads; ++i) {
        SUM += secs[i].sum;
    }

    printf ("integral = %.10f\n", SUM);

    return 0;
}