/*
 *
 * Copyright 2017 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __COMPILER_H__
#define __COMPILER_H__

#if defined(__WIN32) || defined(__WIN64)
#define __packed __attribute__((gcc_struct, packed))
#else
#define __packed __attribute__((packed))
#endif

#define __aligned(x) __attribute__((aligned(x)))
#define __always_unused __attribute__((unused))
#define __must_check __attribute__((warn_unused_result))

/* This evaluates to the type of the first expression, unless that is constant
   in which case it evalutates to the type of the second. This is useful when
   assigning macro parameters to temporary variables, because that would
   normally circumvent the special loosened type promotion rules for integer
   literals. By using this macro, the promotion can happen at the time the
   literal is assigned to the temporary variable. If the literal doesn't fit in
   the chosen type, -Werror=overflow will catch it, so this should be safe. */
#define __TYPEOF_UNLESS_CONST(expr, fallback_expr) typeof( \
	__builtin_choose_expr(__builtin_constant_p(expr), fallback_expr, expr))

/* This creates a unique local variable name for use in macros. */
#define __TMPNAME_3(i) __tmpname_##i
#define __TMPNAME_2(i) __TMPNAME_3(i)
#define __TMPNAME __TMPNAME_2(__COUNTER__)

#endif
