#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1UL)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
#define ALIGN_UP(x,a)           ALIGN((x),(a))
#define ALIGN_DOWN(x,a)         ((x) & ~((typeof(x))(a)-1UL))
#define IS_ALIGNED(x,a)         (((x) & ((typeof(x))(a)-1UL)) == 0)

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ABS(a) (((a) < 0) ? (-(a)) : (a))
#define CEIL_DIV(a, b)  (((a) + (b) - 1) / (b))
#define IS_POWER_OF_2(x)  (((x) & ((x) - 1)) == 0)

#define min(a,b) MIN((a),(b))
#define max(a,b) MAX((a),(b))

#if !defined(__PRE_RAM__)
void *memalign(size_t boundary, size_t size);
void *malloc(size_t size);
/* We never free memory */
static inline void free(void *ptr) {}
#endif

#ifndef __ROMCC__
static inline unsigned long div_round_up(unsigned int n, unsigned int d)
{
	return (n + d - 1) / d;
}
#endif


#endif /* STDLIB_H */
