#ifndef STACK_H
#define STACK_H

#include <time.h>
#include <sys/time.h>

typedef struct stack shared_stack_t;

/* Attach (create if needed) shared memory stack to the process.
Returns mymyshared_stack_t* in case of success. Returns NULL on failure. */
extern shared_stack_t *attach_stack(int key, int size);

/* Detaches existing stack from process.
Operations on detached stack are not permitted since stack pointer becomes
invalid. */
extern int detach_stack(shared_stack_t *stack);

/* Marks stack to be destroed. Destruction are done after all detaches */
extern int mark_destruct(shared_stack_t *stack);

/* Returns stack maximum size. */
extern int get_size(shared_stack_t *stack);

/* Returns current stack size. */
extern int get_count(shared_stack_t *stack);

/* Push val into stack. */
extern int push(shared_stack_t *stack, void *val);

/* Pop val from stack into memory */
extern int pop(shared_stack_t *stack, void **val);

/* Control timeout on push and pop operations in case stack is full or empty.
val == -1 Operations return immediatly, probably with errors.
val == 0  Operations wait infinitely.
val == 1  Operations wait timeout time.
*/
int set_wait(int val, struct timespec *timeout);

#endif
