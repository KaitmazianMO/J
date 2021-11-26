#include "stack.h"
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define KEY 0x777
#define SIZE 10000

int main() {
    int pid = getpid();

    for(int i = 0; i < 10; i++)
        fork();

    shared_stack_t *stack = attach_stack(KEY, SIZE);
    stack = attach_stack(KEY, SIZE);
    assert(stack);

    void *v = 0;

    for (size_t i = 0; i < 3; ++i) {
        v = (void *)i;
    }

    for (size_t i = 0; i < 3; ++i) {
        pop(stack, &v);
    }

    if

    assert(!mark_destruct(stack));
    assert(!detach_stack(stack));

    return 0;
}
