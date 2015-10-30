#ifndef __STABLE_H_INCLUDED
#define __STABLE_H_INCLUDED

#include <stdbool.h>

#include "stack.h"

typedef struct {
    stack_t *stack;
} stable_t;

stable_t *stable_init();
void stable_insert(stable_t *stable, char *key, bst_data_t *data, bool new_scope);
void stable_destroy(stable_t *stable);

#endif
