/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-or-later */

#ifndef COMMONLIB_BSD_CLAMP_H
#define COMMONLIB_BSD_CLAMP_H

#include <stdint.h>

/*
 * Clamp a value, so that it is between a lower and an upper bound.
 */
static inline u32 clamp_u32(const u32 min, const u32 val, const u32 max)
{
	if (val > max)
		return max;

	if (val < min)
		return min;

	return val;
}

#endif /* COMMONLIB_BSD_CLAMP_H */
