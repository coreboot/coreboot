/*
 * This file is part of the libpayload project.
 *
 * Copyright 2014 Google Inc.
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

#include <exception.h>
#include <libpayload.h>
#include <stdint.h>

extern unsigned int test_exc;

struct exception_handler_info
{
	const char *name;
};

static exception_hook hook;
struct exception_state *exception_state;

static struct exception_handler_info exceptions[EXC_COUNT] = {
	[EXC_SYNC_SP0] = { "_sync_sp_el0" },
	[EXC_IRQ_SP0]  = { "_irq_sp_el0" },
	[EXC_FIQ_SP0]  = { "_fiq_sp_el0" },
	[EXC_SERROR_SP0] = {"_serror_sp_el0"},
	[EXC_SYNC_SPX] = { "_sync_spx" },
	[EXC_IRQ_SPX]  = { "_irq_spx" },
	[EXC_FIQ_SPX]  = { "_fiq_spx" },
	[EXC_SERROR_SPX] = {"_serror_spx"},
	[EXC_SYNC_ELX_64] = { "_sync_elx_64" },
	[EXC_IRQ_ELX_64]  = { "_irq_elx_64" },
	[EXC_FIQ_ELX_64]  = { "_fiq_elx_64" },
	[EXC_SERROR_ELX_64] = {"_serror_elx_64"},
	[EXC_SYNC_ELX_32] = { "_sync_elx_32" },
	[EXC_IRQ_ELX_32]  = { "_irq_elx_32" },
	[EXC_FIQ_ELX_32]  = { "_fiq_elx_32" },
	[EXC_SERROR_ELX_32] = {"_serror_elx_32"},
};

static void dump_stack(uintptr_t addr, size_t bytes)
{
	int i, j;
	const int words_per_line = 8;
	uint64_t *ptr = (void *)ALIGN_DOWN(addr, words_per_line * sizeof(*ptr));

	printf("Dumping stack:\n");
	for (i = bytes / sizeof(*ptr); i >= 0; i -= words_per_line) {
		printf("%p: ", ptr + i);
		for (j = i; j < i + words_per_line; j++)
			printf("%016llx ", *(ptr + j));
		printf("\n");
	}
}

static void print_regs(struct exception_state *state)
{
	int i;

	printf("ELR = 0x%016llx         ESR = 0x%08llx\n",
	       state->elr, state->esr);
	printf("FAR = 0x%016llx        SPSR = 0x%08x\n",
	       raw_read_far_current(), raw_read_spsr_current());
	for (i = 0; i < 30; i += 2) {
		printf("X%02d = 0x%016llx         X%02d = 0x%016llx\n",
		       i, state->regs[i], i + 1, state->regs[i + 1]);
	}
	printf("X30 = 0x%016llx          SP = 0x%016llx\n",
	       state->regs[30], raw_read_sp_el0());
}

void exception_dispatch(struct exception_state *state, int idx);
void exception_dispatch(struct exception_state *state, int idx)
{
	exception_state = state;

	if (idx >= EXC_COUNT) {
		printf("Bad exception index %d.\n", idx);
	} else {
		struct exception_handler_info *info = &exceptions[idx];
		if (hook && hook(idx))
			return;

		if (info->name)
			printf("exception %s\n", info->name);
		else
			printf("exception _not_used.\n");
	}
	print_regs(state);
	/* Few words below SP in case we need state from a returned function. */
	dump_stack(raw_read_sp_el0() - 32, 512);

	if (test_exc) {
		state->elr += 4;
		test_exc = 0;
		printf("returning back now\n");
	}
	else
		halt();
}

void exception_init(void)
{
	extern void* exception_table;
	set_vbar(&exception_table);
}

void exception_install_hook(exception_hook h)
{
	die_if(hook, "Implement support for a list of hooks if you need it.");
	hook = h;
}
