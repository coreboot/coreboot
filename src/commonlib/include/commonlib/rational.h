/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COMMONLIB_RATIONAL_H_
#define _COMMONLIB_RATIONAL_H_

#include <stddef.h>

/*
 * Calculate the best rational approximation for a given fraction,
 * with the restriction of maximum numerator and denominator.
 * For example, to find the approximation of 3.1415 with 5 bit denominator
 * and 8 bit numerator fields:
 *
 * rational_best_approximation(31415, 10000,
 *			       (1 << 8) - 1, (1 << 5) - 1, &n, &d);
 */
void rational_best_approximation(
	unsigned long numerator, unsigned long denominator,
	unsigned long max_numerator, unsigned long max_denominator,
	unsigned long *best_numerator, unsigned long *best_denominator);

#endif /* _COMMONLIB_RATIONAL_H_ */
