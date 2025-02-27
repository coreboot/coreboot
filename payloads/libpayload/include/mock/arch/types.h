/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ARCH_TYPES_H
#define _ARCH_TYPES_H

typedef unsigned char uint8_t;
typedef unsigned char uint_fast8_t;
typedef unsigned char u8;
typedef signed char int8_t;
typedef signed char int_fast8_t;
typedef signed char s8;

typedef unsigned short uint16_t;
typedef unsigned short u16;
typedef unsigned short uint_fast16_t;
typedef signed short int16_t;
typedef signed short int_fast16_t;
typedef signed short s16;

typedef unsigned int uint32_t;
typedef unsigned int uint_fast32_t;
typedef unsigned int u32;
typedef signed int int32_t;
typedef signed int int_fast32_t;
typedef signed int s32;

typedef unsigned long long uint64_t;
typedef unsigned long long uint_fast64_t;
typedef unsigned long long u64;
typedef signed long long int64_t;
typedef signed long long int_fast64_t;
typedef signed long long s64;

typedef long time_t;
typedef long suseconds_t;

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif /* _ARCH_TYPES_H */
