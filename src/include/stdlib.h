#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

#define min(a,b) MIN((a),(b))
#define max(a,b) MAX((a),(b))

void *memalign(size_t boundary, size_t size);
void *malloc(size_t size);
/* We never free memory */
static inline void free(void *ptr) {}

#ifndef __ROMCC__
static inline unsigned long div_round_up(unsigned int n, unsigned int d)
{
	return (n + d - 1) / d;
}
#endif


#endif /* STDLIB_H */
