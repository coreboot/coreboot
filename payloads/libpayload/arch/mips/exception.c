/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#include <arch/exception.h>
#include <exception.h>
#include <libpayload.h>
#include <stdint.h>

u32 exception_stack[0x400] __attribute__((aligned(8)));
struct exception_state_t exception_state;

static const char *names[EXC_COUNT] = {
	[EXC_CACHE_ERROR]		= "Cache error exception",
	[EXC_TLB_REFILL_AND_ALL]	= "TLB refill or general exception",
	[EXC_INTERRUPT]			= "Interrupt",
	[EXC_EJTAG_DEBUG]		= "EJTAG debug exception"
};

static void dump_exception_state(void)
{
	printf("%s exception!\n", names[exception_state_ptr->vector]);
	printf("\nRegisters:\n");
	printf("ZERO:\t0x%08x\n", exception_state_ptr->regs.zero);
	printf("AT:\t0x%08x\n", exception_state_ptr->regs.at);
	printf("V0:\t0x%08x\n", exception_state_ptr->regs.v0);
	printf("V1:\t0x%08x\n", exception_state_ptr->regs.v1);
	printf("A0:\t0x%08x\n", exception_state_ptr->regs.a0);
	printf("A1:\t0x%08x\n", exception_state_ptr->regs.a1);
	printf("A2:\t0x%08x\n", exception_state_ptr->regs.a2);
	printf("A3:\t0x%08x\n", exception_state_ptr->regs.a3);
	printf("T0:\t0x%08x\n", exception_state_ptr->regs.t0);
	printf("T1:\t0x%08x\n", exception_state_ptr->regs.t1);
	printf("T2:\t0x%08x\n", exception_state_ptr->regs.t2);
	printf("T3:\t0x%08x\n", exception_state_ptr->regs.t3);
	printf("T4:\t0x%08x\n", exception_state_ptr->regs.t4);
	printf("T5:\t0x%08x\n", exception_state_ptr->regs.t5);
	printf("T6:\t0x%08x\n", exception_state_ptr->regs.t6);
	printf("T7:\t0x%08x\n", exception_state_ptr->regs.t7);
	printf("S0:\t0x%08x\n", exception_state_ptr->regs.s0);
	printf("S1:\t0x%08x\n", exception_state_ptr->regs.s1);
	printf("S2:\t0x%08x\n", exception_state_ptr->regs.s2);
	printf("S3:\t0x%08x\n", exception_state_ptr->regs.s3);
	printf("S4:\t0x%08x\n", exception_state_ptr->regs.s4);
	printf("S5:\t0x%08x\n", exception_state_ptr->regs.s5);
	printf("S6:\t0x%08x\n", exception_state_ptr->regs.s6);
	printf("S7:\t0x%08x\n", exception_state_ptr->regs.s7);
	printf("T8:\t0x%08x\n", exception_state_ptr->regs.t8);
	printf("T9:\t0x%08x\n", exception_state_ptr->regs.t9);
	printf("K0:\t0x%08x\n", exception_state_ptr->regs.k0);
	printf("K1:\t0x%08x\n", exception_state_ptr->regs.k1);
	printf("GP:\t0x%08x\n", exception_state_ptr->regs.gp);
	printf("SP:\t0x%08x\n", exception_state_ptr->regs.sp);
	printf("FP:\t0x%08x\n", exception_state_ptr->regs.fp);
	printf("RA:\t0x%08x\n", exception_state_ptr->regs.ra);
}

static void dump_stack(uintptr_t addr, size_t bytes)
{
	int i, j;
	const int words_per_line = 8;
	int words_to_print;
	uint32_t *ptr = (uint32_t *)
			(addr & ~(words_per_line * sizeof(*ptr) - 1));

	printf("Dumping stack:\n");
	words_to_print = bytes/sizeof(*ptr);
	for (i = words_to_print; i >= 0; i -= words_per_line) {
		printf("%p: ", ptr + i);
		for (j = i; j < i + words_per_line; j++)
			printf("%08x ", *(ptr + j));
		printf("\n");
	}
}


void exception_dispatch(void)
{
	u32 vec = exception_state_ptr->vector;
	die_if(vec >= EXC_COUNT || !names[vec], "Bad exception vector %u", vec);

	dump_exception_state();
	dump_stack(exception_state_ptr->regs.sp, 512);
	halt();
}

void exception_init(void)
{
	exception_stack_end = exception_stack + ARRAY_SIZE(exception_stack);
	exception_state_ptr = &exception_state;
	exception_init_asm();
}
