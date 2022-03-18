/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only */

#ifndef COMMONLIB_BSD_HELPERS_H
#define COMMONLIB_BSD_HELPERS_H

#ifndef __ASSEMBLER__
#include <commonlib/bsd/compiler.h>
#include <stddef.h>
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#define ALIGN(x, a)             __ALIGN_MASK(x, (__typeof__(x))(a)-1UL)
#define __ALIGN_MASK(x, mask)   (((x)+(mask))&~(mask))
#define ALIGN_UP(x, a)          ALIGN((x), (a))
#define ALIGN_DOWN(x, a)        ((x) & ~((__typeof__(x))(a)-1UL))
#define IS_ALIGNED(x, a)        (((x) & ((__typeof__(x))(a)-1UL)) == 0)

/* Double-evaluation unsafe min/max, for bitfields and outside of functions */
#define __CMP_UNSAFE(a, b, op) ((a) op (b) ? (a) : (b))
#define MIN_UNSAFE(a, b) __CMP_UNSAFE(a, b, <)
#define MAX_UNSAFE(a, b) __CMP_UNSAFE(a, b, >)

#define __CMP_SAFE(a, b, op, var_a, var_b) ({ \
	__TYPEOF_UNLESS_CONST(a, b) var_a = (a); \
	__TYPEOF_UNLESS_CONST(b, a) var_b = (b); \
	var_a op var_b ? var_a : var_b; \
})

#define __CMP(a, b, op) __builtin_choose_expr( \
	__builtin_constant_p(a) && __builtin_constant_p(b), \
	__CMP_UNSAFE(a, b, op), __CMP_SAFE(a, b, op, __TMPNAME, __TMPNAME))

#ifndef MIN
#define MIN(a, b) __CMP(a, b, <)
#endif
#ifndef MAX
#define MAX(a, b) __CMP(a, b, >)
#endif

#ifndef ABS
#define ABS(a) ({ \
	__typeof__(a) _abs_local_a = (a); \
	(_abs_local_a < 0) ? (-_abs_local_a) : _abs_local_a; \
})
#endif

#define IS_POWER_OF_2(x) ({ \
	__typeof__(x) _power_local_x = (x); \
	(_power_local_x & (_power_local_x - 1)) == 0; \
})

#define POWER_OF_2(x)		(1ULL << (x))

/* Set bits from `high` to `low` (inclusive). */
#define GENMASK(high, low) (((~0ULL) << (low)) & (~0ULL >> (63 - (high))))

#define DIV_ROUND_UP(x, y) ({ \
	__typeof__(x) _div_local_x = (x); \
	__typeof__(y) _div_local_y = (y); \
	(_div_local_x + _div_local_y - 1) / _div_local_y; \
})

#define SWAP(a, b) do { \
	__typeof__(&(a)) _swap_local_a = &(a); \
	__typeof__(&(b)) _swap_local_b = &(b); \
	__typeof__(a) _swap_local_tmp = *_swap_local_a; \
	*_swap_local_a = *_swap_local_b; \
	*_swap_local_b = _swap_local_tmp; \
} while (0)

/* Standard units. */
#define KiB (1<<10)
#define MiB (1<<20)
#define GiB (1<<30)

#define KHz (1000)
#define MHz (1000 * KHz)
#define GHz (1000 * MHz)

#ifndef offsetof
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)
#endif

#define check_member(structure, member, offset) _Static_assert( \
	offsetof(struct structure, member) == offset, \
	"`struct " #structure "` offset for `" #member "` is not " #offset)

/* Calculate size of structure member. */
#define member_size(type, member)	(sizeof(((type *)0)->member))

#define _retry_impl(attempts, condition, expr, ...)	\
({							\
	__typeof__(condition) _retry_ret =		\
		(__typeof__(condition))0;		\
	int _retry_attempts = (attempts);		\
	do {						\
		_retry_ret = (condition);		\
		if (_retry_ret)				\
			break;				\
		if (--_retry_attempts > 0) {		\
			expr;				\
		} else {				\
			break;				\
		}					\
	} while (1);					\
	_retry_ret;					\
})

/*
 * Helper macro to retry until a condition becomes true or the maximum number
 * of attempts is reached. Two forms are supported:
 *
 * 1. retry(attempts, condition)
 * 2. retry(attempts, condition, expr)
 *
 * @param attempts	Maximum attempts.
 * @param condition	Condition to retry for.
 * @param expr		Procedure to run between each evaluation to "condition".
 *
 * @return Condition value if it evaluates to true within the maximum attempts;
 *	   0 otherwise.
 */
#define retry(attempts, condition, ...) \
	_retry_impl(attempts, condition, __VA_ARGS__)

/* Stringify a token */
#ifndef STRINGIFY
#define _STRINGIFY(x)	#x
#define STRINGIFY(x)	_STRINGIFY(x)
#endif

#endif /* COMMONLIB_BSD_HELPERS_H */
