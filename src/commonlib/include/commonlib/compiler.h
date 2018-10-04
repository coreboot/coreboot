/*
 * This file is part of the coreboot project.
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

#ifndef _COMMONLIB_COMPILER_H_
#define _COMMONLIB_COMPILER_H_

#ifndef __packed
#if defined(__WIN32) || defined(__WIN64)
#define __packed __attribute__((gcc_struct, packed))
#else
#define __packed __attribute__((packed))
#endif
#endif

#ifndef __aligned
#define __aligned(x) __attribute__((aligned(x)))
#endif

#ifndef __always_unused
#define __always_unused __attribute__((unused))
#endif

#ifndef __must_check
#define __must_check __attribute__((warn_unused_result))
#endif

#ifndef __weak
#define __weak __attribute__((weak))
#endif

#ifndef __noreturn
#define __noreturn __attribute__((noreturn))
#endif

#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif

#endif
