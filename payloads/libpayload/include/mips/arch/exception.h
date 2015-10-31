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

#ifndef __MIPS_ARCH_EXCEPTION_H__
#define __MIPS_ARCH_EXCEPTION_H__

#include <stdint.h>

void exception_init_asm(void);
void exception_dispatch(void);

struct exception_state_t {
	struct {
		/* Always 0: just to keep the series complete */
		u32 zero;
		/* Reserved for the assembler */
		/* TODO: is this actually needed here? */
		u32 at;
		/* v0-v1: expression evaluation */
		u32 v0;
		u32 v1;
		/* a0-a3: Arguments */
		u32 a0;
		u32 a1;
		u32 a2;
		u32 a3;
		/* t0-t3: Temporary registers for expression evaluation */
		u32 t0;
		u32 t1;
		u32 t2;
		u32 t3;
		u32 t4;
		u32 t5;
		u32 t6;
		u32 t7;
		/* s0-s7: Saved registers */
		u32 s0;
		u32 s1;
		u32 s2;
		u32 s3;
		u32 s4;
		u32 s5;
		u32 s6;
		u32 s7;
		/* t8-t9: Temporary registers for expression evaluation */
		u32 t8;
		u32 t9;
		/* k0-k1: reserved for SO kernel */
		u32 k0;
		u32 k1;
		/* Global pointer */
		u32 gp;
		/* Stack pointer */
		u32 sp;
		/* Frame pointer */
		u32 fp;
		/* Return address */
		u32 ra;
	} regs;
	u32 vector;
} __attribute__((packed));

extern struct exception_state_t *exception_state_ptr;
extern u32 *exception_stack_end;

enum {
	EXC_CACHE_ERROR = 0,
	EXC_TLB_REFILL_AND_ALL = 1,
	EXC_INTERRUPT = 2,
	EXC_EJTAG_DEBUG = 3,
	EXC_COUNT
};

#endif /* __MIPS_ARCH_EXCEPTION_H__ */
