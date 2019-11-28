/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Myles Watson <mylesgw@gmail.com>
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

/* This file is for "nuisance prototypes" that have no other home. */

#ifndef __LIB_H__
#define __LIB_H__
#include <stdint.h>
#include <types.h>

/* Defined in src/lib/lzma.c. Returns decompressed size or 0 on error. */
size_t ulzman(const void *src, size_t srcn, void *dst, size_t dstn);

/* Defined in src/lib/ramtest.c */
/* Assumption is 32-bit addressable UC memory. */
void ram_check(unsigned long start, unsigned long stop);
int ram_check_nodie(unsigned long start, unsigned long stop);
int ram_check_noprint_nodie(unsigned long start, unsigned long stop);
void quick_ram_check_or_die(uintptr_t dst);

/* Defined in primitive_memtest.c */
int primitive_memtest(uintptr_t base, uintptr_t size);

/* Defined in src/lib/stack.c */
int checkstack(void *top_of_stack, int core);

/*
 * Defined in src/lib/hexdump.c
 * Use the Linux command "xxd" for matching output.  xxd is found in package
 * https://packages.debian.org/jessie/amd64/vim-common/filelist
 */
void hexdump(const void *memory, size_t length);
void hexdump32(char LEVEL, const void *d, size_t len);

/*
 * hexstrtobin - Turn a string of ASCII hex characters into binary
 *
 * @str: String of hex characters to parse
 * @buf: Buffer to store the resulting bytes into
 * @len: Maximum length of buffer to fill
 *
 * Defined in src/lib/hexstrtobin.c
 * Ignores non-hex characters in the string.
 * Returns the number of bytes that have been put in the buffer.
 */
size_t hexstrtobin(const char *str, uint8_t *buf, size_t len);

/* Count Leading Zeroes: clz(0) == 32, clz(0xf) == 28, clz(1 << 31) == 0 */
static inline int clz(u32 x) { return x ? __builtin_clz(x) : sizeof(x) * 8; }
/* Integer binary logarithm (rounding down): log2(0) == -1, log2(5) == 2 */
static inline int log2(u32 x) { return sizeof(x) * 8 - clz(x) - 1; }
/* Find First Set: __ffs(1) == 0, __ffs(0) == -1, __ffs(1<<31) == 31 */
static inline int __ffs(u32 x) { return log2(x & (u32)(-(s32)x)); }

/* Integer binary logarithm (rounding up): log2_ceil(0) == -1, log2(5) == 3 */
static inline int log2_ceil(u32 x) { return (x == 0) ? -1 : log2(x * 2 - 1); }

#endif /* __LIB_H__ */
