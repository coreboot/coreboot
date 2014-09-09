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

#include <stdint.h>
#include <types.h>
#include <arch/cache.h>
#include <arch/exception.h>
#include <arch/transition.h>
#include <console/console.h>
#include <arch/lib_helpers.h>

static unsigned int test_exc;

struct exception_handler_info
{
	const char *name;
};

enum {
	EXC_SYNC_SP0 = 0,
	EXC_IRQ_SP0,
	EXC_FIQ_SP0,
	EXC_SERROR_SP0,
	EXC_SYNC_SP3,
	EXC_IRQ_SP3,
	EXC_FIQ_SP3,
	EXC_SERROR_SP3,
	EXC_SYNC_ELX_64,
	EXC_IRQ_ELX_64,
	EXC_FIQ_ELX_64,
	EXC_SERROR_ELX_64,
	EXC_SYNC_ELX_32,
	EXC_IRQ_ELX_32,
	EXC_FIQ_ELX_32,
	EXC_SERROR_ELX_32,
	EXC_COUNT
};

static struct exception_handler_info exceptions[EXC_COUNT] = {
	[EXC_SYNC_SP0] = { "_sync_sp_el0" },
	[EXC_IRQ_SP0]  = { "_irq_sp_el0" },
	[EXC_FIQ_SP0]  = { "_fiq_sp_el0" },
	[EXC_SERROR_SP0] = {"_serror_sp_el0"},
	[EXC_SYNC_SP3] = { "_sync_sp_el3" },
	[EXC_IRQ_SP3]  = { "_irq_sp_el3" },
	[EXC_FIQ_SP3]  = { "_fiq_sp_el3" },
	[EXC_SERROR_SP3] = {"_serror_sp_el3"},
	[EXC_SYNC_ELX_64] = { "_sync_elx_64" },
	[EXC_IRQ_ELX_64]  = { "_irq_elx_64" },
	[EXC_FIQ_ELX_64]  = { "_fiq_elx_64" },
	[EXC_SERROR_ELX_64] = {"_serror_elx_64"},
	[EXC_SYNC_ELX_32] = { "_sync_elx_32" },
	[EXC_IRQ_ELX_32]  = { "_irq_elx_32" },
	[EXC_FIQ_ELX_32]  = { "_fiq_elx_32" },
	[EXC_SERROR_ELX_32] = {"_serror_elx_32"},
};

static void print_regs(struct exc_state *exc_state)
{
	int i;
	struct elx_state *elx = &exc_state->elx;
	struct regs *regs = &exc_state->regs;

	uint64_t elx_esr = raw_read_esr_current();
	uint64_t elx_far = raw_read_far_current();

	printk(BIOS_DEBUG, "ELR = 0x%016llx\n", elx->elr);
	printk(BIOS_DEBUG, "ESR = 0x%016llx\n", elx_esr);
	printk(BIOS_DEBUG, "SPSR = 0x%08llx\n", elx->spsr);
	printk(BIOS_DEBUG, "FAR = 0x%016llx\n", elx_far);
	for (i = X0_INDEX; i < XMAX_INDEX; i++)
		printk(BIOS_DEBUG, "X%02d = 0x%016llx\n", i, regs->x[i]);
}

void exc_dispatch(struct exc_state *exc_state, uint64_t idx)
{
	if (idx >= EXC_COUNT) {
		printk(BIOS_DEBUG, "Bad exception index %lx.\n",
		       (unsigned long)idx);
	} else {
		struct exception_handler_info *info = &exceptions[idx];

		if (info->name)
			printk(BIOS_DEBUG, "exception %s\n", info->name);
		else
			printk(BIOS_DEBUG, "exception _not_used.\n");
	}
	print_regs(exc_state);

	if (test_exc) {
		exc_state->elx.elr += 4;
		raw_write_elr_current(exc_state->elx.elr);
		test_exc = 0;
		printk(BIOS_DEBUG, "new ELR = 0x%016llx\n", exc_state->elx.elr);
	} else
		die("exception");

	exc_exit(&exc_state->regs);
}

static uint64_t test_exception(void)
{
	uint64_t *a = (uint64_t *)0xfffffffff0000000ULL;

	test_exc = 1;

	printk(BIOS_DEBUG, "%llx\n", *a);

	return 0;
}

void exception_hwinit(void)
{
	exc_set_vbar();
}

void exception_init(void)
{
	/* Load the exception table. */
	exception_hwinit();

	printk(BIOS_DEBUG, "ARM64: Exception handlers installed.\n");

	printk(BIOS_DEBUG, "ARM64: Testing exception\n");
	test_exception();
	printk(BIOS_DEBUG, "ARM64: Done test exception\n");
}
