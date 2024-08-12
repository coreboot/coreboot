/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#include <stdint.h>

struct trapframe {
	uintptr_t gpr[32];
	uintptr_t status;
	uintptr_t epc;
	uintptr_t badvaddr;
	uintptr_t cause;
	uintptr_t insn;
};

typedef uint32_t insn_t;

struct insn_fetch {
	uintptr_t error;
	insn_t insn;
};

static inline void exception_init(void)
{
}

void redirect_trap(void);
void trap_handler(struct trapframe *tf);
void handle_supervisor_call(struct trapframe *tf);

#endif
