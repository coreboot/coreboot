/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-or-later */

#ifndef COMMONLIB_BSD_CLAMP_H
#define COMMONLIB_BSD_CLAMP_H

#include <stdint.h>

/*
 * Clamp a value, so that it is between a lower and an upper bound.
 */
#define __MAKE_CLAMP_FUNC(type) \
	static inline type clamp_##type(const type min, const type val, const type max) \
	{				\
		if (val > max)		\
			return max;	\
		if (val < min)		\
			return min;	\
		return val;		\
	}				\

__MAKE_CLAMP_FUNC(s8)	/* clamp_s8  */
__MAKE_CLAMP_FUNC(u8)	/* clamp_u8  */
__MAKE_CLAMP_FUNC(s16)	/* clamp_s16 */
__MAKE_CLAMP_FUNC(u16)	/* clamp_u16 */
__MAKE_CLAMP_FUNC(s32)	/* clamp_s32 */
__MAKE_CLAMP_FUNC(u32)	/* clamp_u32 */
__MAKE_CLAMP_FUNC(s64)	/* clamp_s64 */
__MAKE_CLAMP_FUNC(u64)	/* clamp_u64 */

#undef __MAKE_CLAMP_FUNC

#endif /* COMMONLIB_BSD_CLAMP_H */
