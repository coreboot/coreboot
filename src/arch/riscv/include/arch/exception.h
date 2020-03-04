/* SPDX-License-Identifier: BSD-3-Clause */
/* This file is part of the coreboot project. */

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#include <stdint.h>

typedef struct {
	uintptr_t gpr[32];
	uintptr_t status;
	uintptr_t epc;
	uintptr_t badvaddr;
	uintptr_t cause;
	uintptr_t insn;
} trapframe;

typedef uint32_t insn_t;

typedef struct {
	uintptr_t error;
	insn_t insn;
} insn_fetch_t;

static inline void exception_init(void)
{
}

void redirect_trap(void);
void trap_handler(trapframe *tf);
void handle_supervisor_call(trapframe *tf);
void handle_misaligned(trapframe *tf);

#endif
