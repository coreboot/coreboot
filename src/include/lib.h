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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

/* This file is for "nuisance prototypes" that have no other home. */

#ifndef __LIB_H__
#define __LIB_H__
#include <stdint.h>
#include <types.h>

#if !defined(__ROMCC__) /* Conflicts with inline function in arch/io.h */
/* Defined in src/lib/clog2.c */
unsigned long log2(unsigned long x);
#endif
unsigned long log2_ceil(unsigned long x);

/* Defined in src/lib/lzma.c */
unsigned long ulzma(unsigned char *src, unsigned char *dst);

/* Defined in src/lib/ramtest.c */
void ram_check(unsigned long start, unsigned long stop);
int ram_check_nodie(unsigned long start, unsigned long stop);
int ram_check_noprint_nodie(unsigned long start, unsigned long stop);
void quick_ram_check(void);

/* Defined in primitive_memtest.c */
int primitive_memtest(uintptr_t base, uintptr_t size);

/* Defined in src/lib/stack.c */
int checkstack(void *top_of_stack, int core);

#ifndef __PRE_RAM__ // fails in bootblock compiled with romcc
/* currently defined by a ldscript */
extern unsigned char _estack[];
#endif

/* Defined in src/lib/hexdump.c */
void hexdump(const void *memory, size_t length);
void hexdump32(char LEVEL, const void *d, size_t len);

#endif /* __LIB_H__ */
