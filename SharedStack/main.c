#include "stack.h"
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define KEY 0x777
#define SIZE 0x100

int main() {

    shared_stack_t *stack = attach_stack(KEY, SIZE);
    stack = attach_stack(KEY, SIZE);
    assert(stack);

    void *v = 0;

    for (size_t i = 0; i < SIZE; ++i) {
        v = (void *)i;
        assert (push(stack, v) == 0);
    }

    printf ("get_count(stack) = %d\n", get_count(stack));
    assert(get_count(stack) == SIZE);

    for (size_t i = 0; i < SIZE; ++i) {
        pop(stack, &v);
        assert(v == (void *)(SIZE - i - 1));
    }

    assert(!mark_destruct(stack));
    assert(!detach_stack(stack));

    return 0;
}
