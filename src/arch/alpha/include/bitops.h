#ifndef ALPHA_BITOPS_H
#define ALPHA_BITOPS_H

/**
 * log2 - Find the truncated log base 2 of x
 */

static inline unsigned long log2(unsigned long x)
{
	unsigned long r = 63;
	do {
		if (x & (1UL << r)) {
			break;
		}
		r--;
	} while(r > 0);
	if (x == 0) {
		r = -1UL;
	}
	return r;
}
#endif /* ALPHA_BITOPS_H */
