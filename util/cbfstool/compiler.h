/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
 * Copyright (c) 2017-2018 Eltan B.V.
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

#if !defined(__FreeBSD__)

#if !defined(__packed)
#if defined(__WIN32) || defined(__WIN64)
#define __packed __attribute__((gcc_struct, packed))
#else
#define __packed __attribute__((packed))
#endif
#endif

#if !defined(__aligned)
#define __aligned(x) __attribute__((aligned(x)))
#endif
#endif

#define __always_unused __attribute__((unused))
#define __must_check __attribute__((warn_unused_result))

#endif
