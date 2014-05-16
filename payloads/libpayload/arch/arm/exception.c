/*
 * This file is part of the libpayload project.
 *
 * Copyright 2013 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <arch/cache.h>
#include <arch/exception.h>
#include <exception.h>
#include <libpayload.h>
#include <stdint.h>

u32 exception_stack[0x400] __attribute__((aligned(8)));
struct exception_state exception_state;

static exception_hook hook;
static const char *names[EXC_COUNT] = {
	[EXC_UNDEF]  = "Undefined Instruction",
	[EXC_SWI]    = "Software Interrupt",
	[EXC_PABORT] = "Prefetch Abort",
	[EXC_DABORT] = "Data Abort",
	[EXC_IRQ]    = "Interrupt",
	[EXC_FIQ]    = "Fast Interrupt",
};

static void dump_stack(uintptr_t addr, size_t bytes)
{
	int i, j;
	const int line = 8;
	uint32_t *ptr = (uint32_t *)(addr & ~(line * sizeof(*ptr) - 1));

	printf("Dumping stack:\n");
	for (i = bytes / sizeof(*ptr); i >= 0; i -= line) {
		printf("%p: ", ptr + i);
		for (j = i; j < i + line; j++)
			printf("%08x ", *(ptr + j));
		printf("\n");
	}
}

static void print_regs(void)
{
	int i;

	for (i = 0; i < 16; i++) {
		if (i == 15)
			printf("PC");
		else if (i == 14)
			printf("LR");
		else if (i == 13)
			printf("SP");
		else if (i == 12)
			printf("IP");
		else
			printf("R%d", i);
		printf(" = 0x%08x\n", exception_state.regs[i]);
	}
	printf("CPSR = 0x%08x\n", exception_state.cpsr);
}

void exception_dispatch(u32 idx)
{
	die_if(idx >= EXC_COUNT || !names[idx], "Bad exception index %u!", idx);

	if (hook && hook(idx))
		return;

	printf("%s Exception\n", names[idx]);
	print_regs();
	dump_stack(exception_state.regs[13], 512);
	halt();
}

void exception_init(void)
{
	uint32_t sctlr = read_sctlr();
	/* Handle exceptions in ARM mode. */
	sctlr &= ~SCTLR_TE;
	/* Set V=0 in SCTLR so VBAR points to the exception vector table. */
	sctlr &= ~SCTLR_V;
	write_sctlr(sctlr);

	extern uint32_t exception_table[];
	set_vbar((uintptr_t)exception_table);

	exception_stack_end = exception_stack + ARRAY_SIZE(exception_stack);
	exception_state_ptr = &exception_state;
}

void exception_install_hook(exception_hook h)
{
	die_if(hook, "Implement support for a list of hooks if you need it.");
	hook = h;
}
