/*
 * This file is part of the coreboot project.
 *
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

#ifndef __SRC_INCLUDE_BASE3_H__
#define __SRC_INCLUDE_BASE3_H__

/* We translate a floating pin (Z) as the ternary digit 2. */
#define Z 2

/*
 * This provides a variadic macro BASE3() that can be used to translate a set of
 * pin states into its base-3 integer representation, even in the context of a
 * static initializer. You can call it with any number of up to 6 arguments,
 * e.g. BASE3(1, Z) -> 5 or BASE3(0, Z, 1, 0) -> 21. Just don't look too closely
 * at how the sausage is made. (Pay extra attention to typos when expanding it!)
 */
#define _BASE3_IMPL_1(arg0, arg1, arg2, arg3, arg4, arg5) arg0
#define _BASE3_IMPL_2(arg0, arg1, arg2, arg3, arg4, arg5) \
	(arg1 + (3 * _BASE3_IMPL_1(arg0, arg1, arg2, arg3, arg4, arg5)))
#define _BASE3_IMPL_3(arg0, arg1, arg2, arg3, arg4, arg5) \
	(arg2 + (3 * _BASE3_IMPL_2(arg0, arg1, arg2, arg3, arg4, arg5)))
#define _BASE3_IMPL_4(arg0, arg1, arg2, arg3, arg4, arg5) \
	(arg3 + (3 * _BASE3_IMPL_3(arg0, arg1, arg2, arg3, arg4, arg5)))
#define _BASE3_IMPL_5(arg0, arg1, arg2, arg3, arg4, arg5) \
	(arg4 + (3 * _BASE3_IMPL_4(arg0, arg1, arg2, arg3, arg4, arg5)))
#define _BASE3_IMPL_6(arg0, arg1, arg2, arg3, arg4, arg5) \
	(arg5 + (3 * _BASE3_IMPL_5(arg0, arg1, arg2, arg3, arg4, arg5)))
#define _BASE3_IMPL(arg0, arg1, arg2, arg3, arg4, arg5, NARGS, ...) \
	_BASE3_IMPL##NARGS(arg0, arg1, arg2, arg3, arg4, arg5)
#define BASE3(...) _BASE3_IMPL(__VA_ARGS__, _6, _5, _4, _3, _2, _1)

#endif /* __SRC_INCLUDE_BASE3_H__ */
