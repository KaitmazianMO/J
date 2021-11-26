#ifndef STACK_H
#define STACK_H

extern struct stack;

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

#endif
