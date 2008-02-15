#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

extern void *malloc(size_t size);
void free(void *ptr);

/* Extensions to malloc... */
typedef size_t malloc_mark_t;
void malloc_mark(malloc_mark_t *place);
void malloc_release(malloc_mark_t *place);

#endif /* STDLIB_H */
