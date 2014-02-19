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
	exception_hook hook;
};

static struct exception_handler_info exceptions[EXC_COUNT] = {
	[EXC_INV]  = { "_invalid_exception" },
	[EXC_SYNC] = { "_sync" },
	[EXC_IRQ]  = { "_irq" },
	[EXC_FIQ]  = { "_fiq" },
	[EXC_SERROR] = {"_serror"}
};

static void print_regs(struct exception_state *state)
{
	int i;

	printf("ELR = 0x%016llx        ",state->elr);
	printf("ESR = 0x%08llx         ",state->esr);
	for (i = 0; i < 31; i++) {
		printf("X%02d = 0x%016llx        ", i, state->regs[i]);
	}
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

	if (test_exc)
		test_exc = 0;
	else
		halt();
}

void exception_init(void)
{
	extern void* exception_table;
	set_vbar(exception_table);
}

void exception_install_hook(int type, exception_hook hook)
{
	die_if(type >= EXC_COUNT, "Out of bounds exception index %d.\n", type);
	exceptions[type].hook = hook;
}
