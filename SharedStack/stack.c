#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>

#include "stack.h"

#define ERR(condition) error(condition, __FILE__, __LINE__, __PRETTY_FUNCTION__)

typedef int data_t;

struct info {
    size_t size;
    size_t capacity;
};

typedef struct stack {
    struct info *info;
    void **data;
    int semid;
    int shmid;
} shared_stack_t;

static struct timespec TIMEOUT = {.tv_sec = 3};

static void sem_wait(int semid);
static void sem_post(int semid);
static void error(int condition, const char *file, int line, const char *func);

shared_stack_t *attach_stack(key_t key, int size) {
    shared_stack_t *stack = calloc(1, sizeof(*stack));
    struct shmid_ds shm_info = {};

    if (!stack) {
        return NULL;
    }
    
    stack->semid = semget(key, 1, IPC_CREAT | 0660);
    ERR(stack->semid == -1);

    sem_wait(stack->semid);

    stack->shmid = shmget(key, sizeof(shared_stack_t) + size * sizeof(void *),
                          IPC_CREAT | 0660);
    ERR(stack->shmid == -1);

    stack->info = shmat(stack->shmid, NULL, 0);
    ERR(stack->info == (void *)-1);

    ERR(shmctl(stack->shmid, IPC_STAT, &shm_info) == -1);

    if (shm_info.shm_nattch == 1) {
        stack->info->size = 0;
        stack->info->capacity = size;
    };

    stack->data = (void **)((char *)stack->info + sizeof(*stack->info));

    sem_post(stack->semid);

    return stack;
}

int detach_stack(shared_stack_t *stack) {
    assert(stack);

    ERR(shmdt(stack->info) == -1);

    free(stack);

    return EXIT_SUCCESS;
}

int mark_destruct(shared_stack_t *stack) {
    assert(stack);

    ERR(shmctl(stack->shmid, IPC_RMID, NULL) == -1);

    return EXIT_SUCCESS;
}

int push(shared_stack_t *stack, void *val) {
    assert(stack);

    sem_wait(stack->semid);

    if (stack->info->size >= stack->info->capacity) {
        sem_post(stack->semid);

        return EXIT_FAILURE;
    }

    stack->data[stack->info->size++] = val;

    sem_post(stack->semid);

    return EXIT_SUCCESS;
}

int pop(shared_stack_t *stack, void **val) {
    assert(stack);
    assert(val);

    sem_wait(stack->semid);

    if (stack->info->size == 0) {
        sem_post(stack->semid);

        return EXIT_FAILURE;
    }

    *val = stack->data[--stack->info->size];

    sem_post(stack->semid);

    return EXIT_SUCCESS;
}

int get_size(shared_stack_t *stack) {
    assert(stack);

    sem_wait(stack->semid);

    int ret = stack->info->capacity;

    sem_post(stack->semid);

    return ret;
}

int get_count(shared_stack_t *stack) {
    assert(stack);

    sem_wait(stack->semid);

    int ret = stack->info->size;

    sem_post(stack->semid);

    return ret;
}

void sem_wait(int semid) {
    ERR(semid == -1);

    struct sembuf sops[2] = {};

    sops[0].sem_num = 0;
    sops[0].sem_op = 0;
    sops[0].sem_flg = 0;
    sops[1].sem_num = 0;
    sops[1].sem_op = 1;
    sops[1].sem_flg = SEM_UNDO;
    ERR(semtimedop(semid, sops, 2, &TIMEOUT) == -1);
}

void sem_post(int semid) {
    ERR(semid == -1);

    struct sembuf sop = {};

    sop.sem_num = 0;
    sop.sem_op = -1;
    sop.sem_flg = SEM_UNDO | IPC_NOWAIT;
    ERR(semop(semid, &sop, 1) == -1);
}

void error(int condition, const char *file, int line, const char *func) {
    assert(file);
    assert(line);
    assert(func);

    if (condition) {
        fprintf(stderr, "ERROR: %s:%d:%s: %s\n", file, line, func,
                strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int set_wait(int val, struct timespec *timeout) {
    switch (val) {
        case -1: {
            TIMEOUT.tv_sec = 0;
            TIMEOUT.tv_nsec = 0;
        }
        case  0: {
            TIMEOUT.tv_sec = 0xFFFFFFFF;
        } break;
        case  1: {
            assert(timeout);
            TIMEOUT = *timeout;
        } break;
    }
    return 0;
}