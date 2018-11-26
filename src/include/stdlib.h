#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

#define min(a, b) MIN((a), (b))
#define max(a, b) MAX((a), (b))

void *memalign(size_t boundary, size_t size);
void *malloc(size_t size);
/* We never free memory */
static inline void free(void *ptr) {}

#endif /* STDLIB_H */
