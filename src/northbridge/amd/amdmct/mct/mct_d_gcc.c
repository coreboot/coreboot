/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

inline void _WRMSR(u32 addr, u32 lo, u32 hi)
{
	__asm__ volatile (
		"wrmsr"
		:
		:"c"(addr),"a"(lo), "d" (hi)
		);
}


inline void _RDMSR(u32 addr, u32 *lo, u32 *hi)
{
	__asm__ volatile (
		"rdmsr"
		:"=a"(*lo), "=d" (*hi)
		:"c"(addr)
		);
}


inline void _RDTSC(u32 *lo, u32 *hi)
{
	__asm__ volatile (
		 "rdtsc"
		 : "=a" (*lo), "=d"(*hi)
		);
}


inline void _cpu_id(u32 addr, u32 *val)
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

#define _MFENCE asm volatile ("mfence")

#define _SFENCE asm volatile ("sfence")

/* prevent speculative execution of following instructions */
#define _EXECFENCE asm volatile ("outb %al, $0xed")

#include <cpu/x86/cr.h>

void proc_CLFLUSH(u32 addr_hi)
{
	SetUpperFSbase(addr_hi);

	__asm__ volatile (
		/* clflush fs:[eax] */
		"outb %%al, $0xed\n\t"	/* _EXECFENCE */
		 "clflush %%fs:(%0)\n\t"
		"mfence\n\t"
		 ::"a" (addr_hi<<8)
	);
}


void WriteLNTestPattern(u32 addr_lo, u8 *buf_a, u32 line_num)
{
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

		 :: "a" (addr_lo), "d" (16), "c" (line_num * 4), "b"(buf_a)
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

#ifdef UNUSED_CODE
static u8 read8_fs(u32 addr_lo)
{
	u8 byte;
	__asm__ volatile (
		"outb %%al, $0xed\n\t"	/* _EXECFENCE */
		"movb %%fs:(%1), %b0\n\t"
		"mfence\n\t"
		:"=b"(byte): "a" (addr_lo)
	);
	return byte;
}
#endif

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

void ReadL18TestPattern(u32 addr_lo)
{
	// set fs and use fs prefix to access the mem
	__asm__ volatile (
		"outb %%al, $0xed\n\t"			/* _EXECFENCE */
		"movl %%fs:-128(%%esi), %%eax\n\t"	//TestAddr cache line
		"movl %%fs:-64(%%esi), %%eax\n\t"	//+1
		"movl %%fs:(%%esi), %%eax\n\t"		//+2
		"movl %%fs:64(%%esi), %%eax\n\t"	//+3

		"movl %%fs:-128(%%edi), %%eax\n\t"	//+4
		"movl %%fs:-64(%%edi), %%eax\n\t"	//+5
		"movl %%fs:(%%edi), %%eax\n\t"		//+6
		"movl %%fs:64(%%edi), %%eax\n\t"	//+7

		"movl %%fs:-128(%%ebx), %%eax\n\t"	//+8
		"movl %%fs:-64(%%ebx), %%eax\n\t"	//+9
		"movl %%fs:(%%ebx), %%eax\n\t"		//+10
		"movl %%fs:64(%%ebx), %%eax\n\t"	//+11

		"movl %%fs:-128(%%ecx), %%eax\n\t"	//+12
		"movl %%fs:-64(%%ecx), %%eax\n\t"	//+13
		"movl %%fs:(%%ecx), %%eax\n\t"		//+14
		"movl %%fs:64(%%ecx), %%eax\n\t"	//+15

		"movl %%fs:-128(%%edx), %%eax\n\t"	//+16
		"movl %%fs:-64(%%edx), %%eax\n\t"	//+17
		"mfence\n\t"

		 :: "a"(0), "b" (addr_lo+128+8*64), "c" (addr_lo+128+12*64),
		    "d" (addr_lo +128+16*64), "S"(addr_lo+128),
		    "D"(addr_lo+128+4*64)
	);

}

void ReadL9TestPattern(u32 addr_lo)
{

	// set fs and use fs prefix to access the mem
	__asm__ volatile (
		"outb %%al, $0xed\n\t"			/* _EXECFENCE */

		"movl %%fs:-128(%%ecx), %%eax\n\t"	//TestAddr cache line
		"movl %%fs:-64(%%ecx), %%eax\n\t"	//+1
		"movl %%fs:(%%ecx), %%eax\n\t"		//+2
		"movl %%fs:64(%%ecx), %%eax\n\t"	//+3

		"movl %%fs:-128(%%edx), %%eax\n\t"	//+4
		"movl %%fs:-64(%%edx), %%eax\n\t"	//+5
		"movl %%fs:(%%edx), %%eax\n\t"		//+6
		"movl %%fs:64(%%edx), %%eax\n\t"	//+7

		"movl %%fs:-128(%%ebx), %%eax\n\t"	//+8
		"mfence\n\t"

		 :: "a"(0), "b" (addr_lo+128+8*64), "c"(addr_lo+128),
		    "d"(addr_lo+128+4*64)
	);

}

void ReadMaxRdLat1CLTestPattern_D(u32 addr)
{
	SetUpperFSbase(addr);

	__asm__ volatile (
		"outb %%al, $0xed\n\t"			/* _EXECFENCE */
		"movl %%fs:-128(%%esi), %%eax\n\t"	//TestAddr cache line
		"movl %%fs:-64(%%esi), %%eax\n\t"	//+1
		"movl %%fs:(%%esi), %%eax\n\t"		//+2
		"mfence\n\t"
		 :: "a"(0), "S"((addr<<8)+128)
	);

}

void WriteMaxRdLat1CLTestPattern_D(u32 buf, u32 addr)
{
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

		 :: "a" (addr<<8), "d" (16), "c" (3 * 4), "b"(buf)
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
		"clflush %%fs:-128(%%esi)\n\t"	 //TestAddr cache line
		"clflush %%fs:-64(%%esi)\n\t"	 //+1
		"clflush %%fs:(%%esi)\n\t"  //+2
		"mfence\n\t"

		 :: "S"((addr<<8)+128)
	);
}

u32 stream_to_int(u8 const *p)
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
