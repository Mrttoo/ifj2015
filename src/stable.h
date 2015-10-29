#ifndef __STABLE_H_INCLUDED
#define __STABLE_H_INCLUDED

#include "stack.h"

typedef struct {
    stack_t *stack;
} stable_t;

stable_t *stable_init();
void stable_destroy(stable_t *stable);


#endif
