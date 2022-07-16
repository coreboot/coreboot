/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only */

#ifndef _COMMONLIB_BSD_COMPILER_H_
#define _COMMONLIB_BSD_COMPILER_H_

#ifndef __packed
#if defined(__WIN32) || defined(__WIN64)
#define __packed __attribute__((__gcc_struct__, __packed__))
#else
#define __packed __attribute__((__packed__))
#endif
#endif

#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

/*
 * Because there may be variables/parameters whose name contains "__unused" in
 * header files of libc, namely musl, names consistent with the ones in the
 * Linux kernel may be a better choice.
 */

/*
 * This is used to mark identifiers unused in all conditions, e.g. a parameter
 * completely unused in all code branch, only present to fit an API.
 */
#ifndef __always_unused
#define __always_unused __attribute__((__unused__))
#endif

/*
 * This is used to mark identifiers unused in some conditions, e.g. a parameter
 * only unused in some code branches, a global variable only accessed with code
 * being conditionally preprocessed, etc.
 */
#ifndef __maybe_unused
#define __maybe_unused __attribute__((__unused__))
#endif

#ifndef __must_check
#define __must_check __attribute__((__warn_unused_result__))
#endif

#ifndef __weak
#define __weak __attribute__((__weak__))
#endif

#ifndef __noreturn
#define __noreturn __attribute__((__noreturn__))
#endif

#ifndef __always_inline
#define __always_inline inline __attribute__((__always_inline__))
#endif

#ifndef __fallthrough
#define __fallthrough __attribute__((__fallthrough__))
#endif

/*
 * This evaluates to the type of the first expression, unless that is constant
 * in which case it evaluates to the type of the second. This is useful when
 * assigning macro parameters to temporary variables, because that would
 * normally circumvent the special loosened type promotion rules for integer
 * literals. By using this macro, the promotion can happen at the time the
 * literal is assigned to the temporary variable. If the literal doesn't fit in
 * the chosen type, -Werror=overflow will catch it, so this should be safe.
 */
#define __TYPEOF_UNLESS_CONST(expr, fallback_expr) __typeof__( \
	__builtin_choose_expr(__builtin_constant_p(expr), fallback_expr, expr))

/* This creates a unique local variable name for use in macros. */
#define __TMPNAME_3(i) __tmpname_##i
#define __TMPNAME_2(i) __TMPNAME_3(i)
#define __TMPNAME __TMPNAME_2(__COUNTER__)

#endif
