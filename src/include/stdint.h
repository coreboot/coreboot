/*
 * This file is part of the coreboot project.
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

#ifndef STDINT_H
#define STDINT_H

/* romcc does not support long long, _Static_assert, or _Bool, so we must ifdef that code out.
   Also, GCC can provide its own implementation of stdint.h, so in theory we could use that
   instead of this custom file once romcc is no more. */

/* Fixed width integer types */
typedef signed char        int8_t;
typedef unsigned char      uint8_t;

typedef signed short       int16_t;
typedef unsigned short     uint16_t;

typedef signed int         int32_t;
typedef unsigned int       uint32_t;

typedef signed long long   int64_t;
typedef unsigned long long uint64_t;

/* Types for 'void *' pointers */
typedef signed long        intptr_t;
typedef unsigned long      uintptr_t;

/* Ensure that the widths are all correct */
_Static_assert(sizeof(int8_t) == 1, "Size of int8_t is incorrect");
_Static_assert(sizeof(uint8_t) == 1, "Size of uint8_t is incorrect");

_Static_assert(sizeof(int16_t) == 2, "Size of int16_t is incorrect");
_Static_assert(sizeof(uint16_t) == 2, "Size of uint16_t is incorrect");

_Static_assert(sizeof(int32_t) == 4, "Size of int32_t is incorrect");
_Static_assert(sizeof(uint32_t) == 4, "Size of uint32_t is incorrect");

_Static_assert(sizeof(int64_t) == 8, "Size of int64_t is incorrect");
_Static_assert(sizeof(uint64_t) == 8, "Size of uint64_t is incorrect");

_Static_assert(sizeof(intptr_t) == sizeof(void *), "Size of intptr_t is incorrect");
_Static_assert(sizeof(uintptr_t) == sizeof(void *), "Size of uintptr_t is incorrect");

/* Maximum width integer types */
typedef int64_t  intmax_t;
typedef uint64_t uintmax_t;

/* Convenient typedefs */
typedef int8_t   s8;
typedef uint8_t  u8;

typedef int16_t  s16;
typedef uint16_t u16;

typedef int32_t  s32;
typedef uint32_t u32;

typedef int64_t  s64;
typedef uint64_t u64;

/* Limits of integer types */
#define INT8_MIN   ((int8_t)0x80)
#define INT8_MAX   ((int8_t)0x7F)
#define UINT8_MAX  ((uint8_t)0xFF)

#define INT16_MIN  ((int16_t)0x8000)
#define INT16_MAX  ((int16_t)0x7FFF)
#define UINT16_MAX ((uint16_t)0xFFFF)

#define INT32_MIN  ((int32_t)0x80000000)
#define INT32_MAX  ((int32_t)0x7FFFFFFF)
#define UINT32_MAX ((uint32_t)0xFFFFFFFF)

#define INT64_MIN  ((int64_t)0x8000000000000000)
#define INT64_MAX  ((int64_t)0x7FFFFFFFFFFFFFFF)
#define UINT64_MAX ((uint64_t)0xFFFFFFFFFFFFFFFF)

#define INTMAX_MIN  INT64_MIN
#define INTMAX_MAX  INT64_MAX
#define UINTMAX_MAX UINT64_MAX

#endif /* STDINT_H */
