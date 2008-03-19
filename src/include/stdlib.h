#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#ifndef __ROMCC__
extern void *malloc(size_t size);
void free(void *ptr);

/* Extensions to malloc... */
typedef size_t malloc_mark_t;
void malloc_mark(malloc_mark_t *place);
void malloc_release(malloc_mark_t *place);
#endif

#endif /* STDLIB_H */
