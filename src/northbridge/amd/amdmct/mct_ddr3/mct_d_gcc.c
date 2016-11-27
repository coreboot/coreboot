/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

#include "mct_d_gcc.h"
#include <inttypes.h>
#include <arch/cpu.h>

void _WRMSR(u32 addr, u32 lo, u32 hi)
{
	__asm__ volatile (
		"wrmsr"
		:
		:"c"(addr),"a"(lo), "d" (hi)
		);
}

void _RDMSR(u32 addr, u32 *lo, u32 *hi)
{
	__asm__ volatile (
		"rdmsr"
		:"=a"(*lo), "=d" (*hi)
		:"c"(addr)
		);
}

void _RDTSC(u32 *lo, u32 *hi)
{
	__asm__ volatile (
		 "rdtsc"
		 : "=a" (*lo), "=d"(*hi)
		);
}

void _cpu_id(u32 addr, u32 *val)
{
	__asm__ volatile(
		 "cpuid"
		 : "=a" (val[0]),
		   "=b" (val[1]),
		   "=c" (val[2]),
		   "=d" (val[3])
		 : "0" (addr));

}

u32 bsr(u32 x)
{
	u8 i;
	u32 ret = 0;

	for (i = 31; i > 0; i--) {
		if (x & (1<<i)) {
			ret = i;
			break;
		}
	}

	return ret;

}

u32 bsf(u32 x)
{
	u8 i;
	u32 ret = 32;

	for (i = 0; i < 32; i++) {
		if (x & (1<<i)) {
			ret = i;
			break;
		}
	}

	return ret;
}

void proc_MFENCE(void)
{
	__asm__ volatile (
		"outb %%al, $0xed\n\t"  /* _EXECFENCE */
		"mfence\n\t"
		:::"memory"
	);
}

void proc_CLFLUSH(u32 addr_hi)
{
	SetUpperFSbase(addr_hi);

	__asm__ volatile (
		/* clflush fs:[eax] */
		"outb	%%al, $0xed\n\t"	/* _EXECFENCE */
		"clflush	%%fs:(%0)\n\t"
		"mfence\n\t"
		::"a" (addr_hi<<8)
	);
}


void WriteLNTestPattern(u32 addr_lo, u8 *buf_a, u32 line_num)
{
	uint32_t step = 16;
	uint32_t count = line_num * 4;

	__asm__ volatile (
		/*prevent speculative execution of following instructions*/
		/* FIXME: needed ? */
		"outb %%al, $0xed\n\t"	/* _EXECFENCE */
		"1:\n\t"
		"movdqa (%3), %%xmm0\n\t"
		"movntdq %%xmm0, %%fs:(%0)\n\t"	/* xmm0 is 128 bit */
		"addl %1, %0\n\t"
		"addl %1, %3\n\t"
		"loop 1b\n\t"
		"mfence\n\t"

		 : "+a" (addr_lo), "+d" (step), "+c" (count), "+b" (buf_a) : :
	);

}

u32 read32_fs(u32 addr_lo)
{
	u32 value;
	__asm__ volatile (
		"outb %%al, $0xed\n\t"	/* _EXECFENCE */
		"movl %%fs:(%1), %0\n\t"
		:"=b"(value): "a" (addr_lo)
	);
	return value;
}

uint64_t read64_fs(uint32_t addr_lo)
{
	uint64_t value = 0;
	uint32_t value_lo;
	uint32_t value_hi;

	__asm__ volatile (
		"outb %%al, $0xed\n\t"  /* _EXECFENCE */
		"mfence\n\t"
		"movl %%fs:(%2), %0\n\t"
		"movl %%fs:(%3), %1\n\t"
		:"=c"(value_lo), "=d"(value_hi): "a" (addr_lo), "b" (addr_lo + 4) : "memory"
	);
	value |= value_lo;
	value |= ((uint64_t)value_hi) << 32;
	return value;
}

void FlushDQSTestPattern_L9(u32 addr_lo)
{
	__asm__ volatile (
		"outb %%al, $0xed\n\t"	/* _EXECFENCE */
		"clflush %%fs:-128(%%ecx)\n\t"
		"clflush %%fs:-64(%%ecx)\n\t"
		"clflush %%fs:(%%ecx)\n\t"
		"clflush %%fs:64(%%ecx)\n\t"

		"clflush %%fs:-128(%%eax)\n\t"
		"clflush %%fs:-64(%%eax)\n\t"
		"clflush %%fs:(%%eax)\n\t"
		"clflush %%fs:64(%%eax)\n\t"

		"clflush %%fs:-128(%%ebx)\n\t"

		 ::  "b" (addr_lo+128+8*64), "c"(addr_lo+128),
		     "a"(addr_lo+128+4*64)
	);

}

__attribute__((noinline)) void FlushDQSTestPattern_L18(u32 addr_lo)
{
	__asm__ volatile (
		"outb %%al, $0xed\n\t"	/* _EXECFENCE */
		"clflush %%fs:-128(%%eax)\n\t"
		"clflush %%fs:-64(%%eax)\n\t"
		"clflush %%fs:(%%eax)\n\t"
		"clflush %%fs:64(%%eax)\n\t"

		"clflush %%fs:-128(%%edi)\n\t"
		"clflush %%fs:-64(%%edi)\n\t"
		"clflush %%fs:(%%edi)\n\t"
		"clflush %%fs:64(%%edi)\n\t"

		"clflush %%fs:-128(%%ebx)\n\t"
		"clflush %%fs:-64(%%ebx)\n\t"
		"clflush %%fs:(%%ebx)\n\t"
		"clflush %%fs:64(%%ebx)\n\t"

		"clflush %%fs:-128(%%ecx)\n\t"
		"clflush %%fs:-64(%%ecx)\n\t"
		"clflush %%fs:(%%ecx)\n\t"
		"clflush %%fs:64(%%ecx)\n\t"

		"clflush %%fs:-128(%%edx)\n\t"
		"clflush %%fs:-64(%%edx)\n\t"

		 :: "b" (addr_lo+128+8*64), "c" (addr_lo+128+12*64),
		    "d" (addr_lo +128+16*64), "a"(addr_lo+128),
		    "D"(addr_lo+128+4*64)
	);
}

void ReadMaxRdLat1CLTestPattern_D(u32 addr)
{
	SetUpperFSbase(addr);

	__asm__ volatile (
		"outb %%al, $0xed\n\t"			/* _EXECFENCE */
		"movl %%fs:-128(%%esi), %%eax\n\t"	/* TestAddr cache line */
		"movl %%fs:-64(%%esi), %%eax\n\t"	/* +1 */
		"movl %%fs:(%%esi), %%eax\n\t"		/* +2 */
		"mfence\n\t"
		 :: "a"(0), "S"((addr<<8)+128)
	);

}

void WriteMaxRdLat1CLTestPattern_D(u32 buf, u32 addr)
{
	uint32_t addr_phys = addr << 8;
	uint32_t step = 16;
	uint32_t count = 3 * 4;

	SetUpperFSbase(addr);

	__asm__ volatile (
		"outb %%al, $0xed\n\t"	/* _EXECFENCE */
		"1:\n\t"
		"movdqa (%3), %%xmm0\n\t"
		"movntdq %%xmm0, %%fs:(%0)\n\t" /* xmm0 is 128 bit */
		"addl %1, %0\n\t"
		"addl %1, %3\n\t"
		"loop 1b\n\t"
		"mfence\n\t"

		 : "+a" (addr_phys), "+d" (step), "+c" (count), "+b" (buf) : :
	);
}

void FlushMaxRdLatTestPattern_D(u32 addr)
{
	/*  Flush a pattern of 72 bit times (per DQ) from cache.
	 * This procedure is used to ensure cache miss on the next read training.
	 */

	SetUpperFSbase(addr);

	__asm__ volatile (
		"outb %%al, $0xed\n\t"	/* _EXECFENCE */
		"clflush %%fs:-128(%%esi)\n\t"	 /* TestAddr cache line */
		"clflush %%fs:-64(%%esi)\n\t"	 /* +1 */
		"clflush %%fs:(%%esi)\n\t"  /* +2 */
		"mfence\n\t"

		 :: "S"((addr<<8)+128)
	);
}

u32 stream_to_int(u8 *p)
{
	int i;
	u32 val;
	u32 valx;

	val = 0;

	for (i = 3; i >= 0; i--) {
		val <<= 8;
		valx = *(p+i);
		val |= valx;
	}

	return val;
}

u8 oemNodePresent_D(u8 Node, u8 *ret)
{
	*ret = 0;
	return 0;
}
