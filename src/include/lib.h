/* SPDX-License-Identifier: GPL-2.0-only */

/* This file is for "nuisance prototypes" that have no other home. */

#ifndef __LIB_H__
#define __LIB_H__

#include <types.h>

/* Defined in src/lib/lzma.c. Returns decompressed size or 0 on error. */
size_t ulzman(const void *src, size_t srcn, void *dst, size_t dstn);

/* Defined in src/lib/ramtest.c */
/* Assumption is 32-bit addressable UC memory. */
void ram_check(uintptr_t start);
int ram_check_nodie(uintptr_t start);
int ram_check_noprint_nodie(uintptr_t start);
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
 * Ignores the last hex character if the number of hex characters in the string
 * is odd.
 * Returns the number of bytes that have been put in the buffer.
 */
size_t hexstrtobin(const char *str, uint8_t *buf, size_t len);

/* Population Count: number of bits that are one */
static inline int popcnt(u32 x) { return __builtin_popcount(x); }
/* Count Leading Zeroes: clz(0) == 32, clz(0xf) == 28, clz(1 << 31) == 0 */
static inline int clz(u32 x) { return x ? __builtin_clz(x) : sizeof(x) * 8; }
/* Integer binary logarithm (rounding down): log2(0) == -1, log2(5) == 2 */
static inline int log2(u32 x) { return sizeof(x) * 8 - clz(x) - 1; }
/* Find First Set: __ffs(1) == 0, __ffs(0) == -1, __ffs(1<<31) == 31 */
static inline int __ffs(u32 x) { return log2(x & (u32)(-(s32)x)); }

/* Integer binary logarithm (rounding up): log2_ceil(0) == -1, log2(5) == 3 */
static inline int log2_ceil(u32 x) { return (x == 0) ? -1 : log2(x * 2 - 1); }

static inline int popcnt64(u64 x) { return __builtin_popcountll(x); }
static inline int clz64(u64 x) { return x ? __builtin_clzll(x) : sizeof(x) * 8; }
static inline int log2_64(u64 x) { return sizeof(x) * 8 - clz64(x) - 1; }
static inline int __ffs64(u64 x) { return log2_64(x & (u64)(-(s64)x)); }

#endif /* __LIB_H__ */
