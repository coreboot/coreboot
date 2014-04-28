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
#include <console/console.h>

void exception_sync_el0(uint64_t *regs, uint64_t esr);
void exception_irq_el0(uint64_t *regs, uint64_t esr);
void exception_fiq_el0(uint64_t *regs, uint64_t esr);
void exception_serror_el0(uint64_t *regs, uint64_t esr);
void exception_sync(uint64_t *regs, uint64_t esr);
void exception_irq(uint64_t *regs, uint64_t esr);
void exception_fiq(uint64_t *regs, uint64_t esr);
void exception_serror(uint64_t *regs, uint64_t esr);

static void print_regs(uint64_t *regs)
{
	int i;

	/* ELR contains the restart PC at target exception level */
	printk(BIOS_ERR, "ELR = 0x%016llx        ", regs[0]);
	printk(BIOS_ERR, "X00 = 0x%016llx\n", regs[1]);

	for (i = 2; i < 31; i+=2) {
		printk(BIOS_ERR, "X%02d = 0x%016llx        ", i - 1, regs[i]);
		printk(BIOS_ERR, "X%02d = 0x%016llx\n", i, regs[i + 1]);
	}
}

void exception_sync_el0(uint64_t *regs, uint64_t esr)
{
	printk(BIOS_ERR, "exception _sync_el0 (ESR = 0x%08llx)\n", esr);
	print_regs(regs);
	die("exception");
}

void exception_irq_el0(uint64_t *regs, uint64_t esr)
{
	printk(BIOS_ERR, "exception _irq_el0 (ESR = 0x%08llx)\n", esr);
	print_regs(regs);
	die("exception");
}

void exception_fiq_el0(uint64_t *regs, uint64_t esr)
{
	printk(BIOS_ERR, "exception _fiq_el0 (ESR = 0x%08llx)\n", esr);
	print_regs(regs);
	die("exception");
}

void exception_serror_el0(uint64_t *regs, uint64_t esr)
{
	printk(BIOS_ERR, "exception _serror_el0 (ESR = 0x%08llx)\n", esr);
	print_regs(regs);
	die("exception");
}

void exception_sync(uint64_t *regs, uint64_t esr)
{
	printk(BIOS_ERR, "exception _sync (ESR = 0x%08llx)\n", esr);
	print_regs(regs);
	die("exception");
}

void exception_irq(uint64_t *regs, uint64_t esr)
{
	printk(BIOS_ERR, "exception _irq (ESR = 0x%08llx)\n", esr);
	print_regs(regs);
	die("exception");
}

void exception_fiq(uint64_t *regs, uint64_t esr)
{
	printk(BIOS_ERR, "exception _fiq (ESR = 0x%08llx)\n", esr);
	print_regs(regs);
	die("exception");
}

void exception_serror(uint64_t *regs, uint64_t esr)
{
	printk(BIOS_ERR, "exception _serror (ESR = 0x%08llx)\n", esr);
	print_regs(regs);
	die("exception");
}

void exception_init(void)
{
	//uint32_t sctlr = read_sctlr();
	/* Handle exceptions in ARM mode. */
	//sctlr &= ~SCTLR_TE;
	/* Set V=0 in SCTLR so VBAR points to the exception vector table. */
	//sctlr &= ~SCTLR_V;
	/* Enforce alignment temporarily. */
	//write_sctlr(sctlr);

	extern uint32_t exception_table[];
	set_vbar((uintptr_t)exception_table);

	printk(BIOS_DEBUG, "Exception handlers installed.\n");
}
