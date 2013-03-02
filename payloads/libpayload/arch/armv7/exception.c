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

#include <arch/exception.h>
#include <libpayload.h>
#include <stdint.h>

void exception_test(void);

static int test_abort;

void exception_undefined_instruction(uint32_t *);
void exception_software_interrupt(uint32_t *);
void exception_prefetch_abort(uint32_t *);
void exception_data_abort(uint32_t *);
void exception_not_used(uint32_t *);
void exception_irq(uint32_t *);
void exception_fiq(uint32_t *);

static void print_regs(uint32_t *regs)
{
	int i;
	/* Don't print the link register and stack pointer since we don't have their
	 * actual value. They are hidden by the 'shadow' registers provided
	 * by the trap hardware.
	 */
	for (i = 0; i < 16; i++) {
		if (i == 15)
			printf("PC");
		else if (i == 14)
			continue; /* LR */
		else if (i == 13)
			continue; /* SP */
		else if (i == 12)
			printf("IP");
		else
			printf("R%d", i);
		printf(" = 0x%08x\n", regs[i]);
	}
}

void exception_undefined_instruction(uint32_t *regs)
{
	printf("exception _undefined_instruction\n");
	print_regs(regs);
	halt();
}

void exception_software_interrupt(uint32_t *regs)
{
	printf("exception _software_interrupt\n");
	print_regs(regs);
	halt();
}

void exception_prefetch_abort(uint32_t *regs)
{
	printf("exception _prefetch_abort\n");
	print_regs(regs);
	halt();
}

void exception_data_abort(uint32_t *regs)
{
	if (test_abort) {
		regs[15] = regs[0];
		return;
	} else {
		printf("exception _data_abort\n");
		print_regs(regs);
	}
	halt();
}

void exception_not_used(uint32_t *regs)
{
	printf("exception _not_used\n");
	print_regs(regs);
	halt();
}

void exception_irq(uint32_t *regs)
{
	printf("exception _irq\n");
	print_regs(regs);
	halt();
}

void exception_fiq(uint32_t *regs)
{
	printf("exception _fiq\n");
	print_regs(regs);
	halt();
}

static inline uint32_t get_sctlr(void)
{
	uint32_t val;
	asm("mrc p15, 0, %0, c1, c0, 0" : "=r" (val));
	return val;
}

static inline void set_sctlr(uint32_t val)
{
	asm volatile("mcr p15, 0, %0, c1, c0, 0" :: "r" (val));
	asm volatile("" ::: "memory");
}

void exception_init(void)
{
	static const uint32_t sctlr_te = (0x1 << 30);
	static const uint32_t sctlr_v = (0x1 << 13);
	static const uint32_t sctlr_a = (0x1 << 1);

	uint32_t sctlr = get_sctlr();
	/* Handle exceptions in ARM mode. */
	sctlr &= ~sctlr_te;
	/* Set V=0 in SCTLR so VBAR points to the exception vector table. */
	sctlr &= ~sctlr_v;
	/* Enforce alignment. */
	sctlr |= sctlr_a;
	set_sctlr(sctlr);

	extern uint32_t exception_table[];
	set_vbar((uintptr_t)exception_table);

	test_abort = 1;
	exception_test();
	test_abort = 0;
}
