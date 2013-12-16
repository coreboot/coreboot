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

uint8_t exception_stack[0x1000] __attribute__((aligned(8)));
extern void *exception_stack_end;

struct exception_handler_info
{
	const char *name;
	exception_hook hook;
};

static struct exception_handler_info exceptions[EXC_COUNT] = {
	[EXC_UNDEF]  = { "_undefined_instruction" },
	[EXC_SWI]    = { "_software_interrupt" },
	[EXC_PABORT] = { "_prefetch_abort" },
	[EXC_DABORT] = { "_data_abort" },
	[EXC_IRQ]    = { "_irq" },
	[EXC_FIQ]    = { "_fiq" },
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

static void print_regs(struct exception_state *state)
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
		printf(" = 0x%08x\n", state->regs[i]);
	}
	printf("CPSR = 0x%08x\n", state->cpsr);
}

void exception_dispatch(struct exception_state *state, int idx);
void exception_dispatch(struct exception_state *state, int idx)
{
	if (idx >= EXC_COUNT) {
		printf("Bad exception index %d.\n", idx);
	} else {
		struct exception_handler_info *info = &exceptions[idx];
		if (info->hook) {
			info->hook(idx, state);
			return;
		}

		if (info->name)
			printf("exception %s\n", info->name);
		else
			printf("exception _not_used.\n");
	}
	print_regs(state);
	dump_stack(state->regs[13], 512);
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
	exception_stack_end = exception_stack + sizeof(exception_stack);
}

void exception_install_hook(int type, exception_hook hook)
{
	die_if(type >= EXC_COUNT, "Out of bounds exception index %d.\n", type);
	exceptions[type].hook = hook;
}
