/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef COMMONLIB_HELPERS_H
#define COMMONLIB_HELPERS_H

/* This file is for helpers for both coreboot firmware and its utilities. Most
   of this has moved into <commonlib/bsd/helpers.h> now, this wrapper is just
   for the stuff that nobody bothered to confirm BSD-licensability of yet. */

#include <commonlib/bsd/helpers.h>

/*
 * Divide positive or negative dividend by positive divisor and round
 * to closest integer. Result is undefined for negative divisors and
 * for negative dividends if the divisor variable type is unsigned.
 */
#define DIV_ROUND_CLOSEST(x, divisor)({					\
	__typeof__(x) _div_local_x = (x);				\
	__typeof__(divisor) _div_local_d = (divisor);			\
	(((__typeof__(x))-1) > 0 ||					\
	 ((__typeof__(divisor))-1) > 0 || (_div_local_x) > 0) ?		\
		((_div_local_x + (_div_local_d / 2)) / _div_local_d) :	\
		((_div_local_x - (_div_local_d / 2)) / _div_local_d);	\
})

/**
 * container_of - cast a member of a structure out to the containing structure
 * @param ptr:    the pointer to the member.
 * @param type:   the type of the container struct this is embedded in.
 * @param member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const __typeof__(((type *)0)->member) *__mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })

#ifndef alloca
#define alloca(x) __builtin_alloca(x)
#endif

#endif /* COMMONLIB_HELPERS_H */
