#ifndef I386_BITOPS_H
#define I386_BITOPS_H

/**
 * log2 - Find the truncated log base 2 of x
 */

static inline unsigned long log2(unsigned long x)
{
	unsigned long r = 0;
	__asm__(
		"bsrl %1, %0\n\t"
		"jnz 1f\n\t"
		"movl $-1, %0\n\t"
		"1:\n\t"
		: "=r" (r) : "r" (x));
	return r;

}
#endif /* I386_BITOPS_H */
