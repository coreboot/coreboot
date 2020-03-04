/* SPDX-License-Identifier: BSD-3-Clause */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <types.h>
#include <arch/cache.h>
#include <arch/exception.h>
#include <console/console.h>

uint8_t exception_stack[0x100] __attribute__((aligned(8)));
extern void *exception_stack_end;

void exception_undefined_instruction(uint32_t *);
void exception_software_interrupt(uint32_t *);
void exception_prefetch_abort(uint32_t *);
void exception_data_abort(uint32_t *);
void exception_not_used(uint32_t *);
void exception_irq(uint32_t *);
void exception_fiq(uint32_t *);

static void dump_stack(uintptr_t addr, size_t bytes)
{
	int i, j;
	const int line = 8;
	uint32_t *ptr = (uint32_t *)(addr & ~(line * sizeof(*ptr) - 1));

	printk(BIOS_ERR, "Dumping stack:\n");
	for (i = bytes / sizeof(*ptr); i >= 0; i -= line) {
		printk(BIOS_ERR, "%p: ", ptr + i);
		for (j = i; j < i + line; j++)
			printk(BIOS_ERR, "%08x ", *(ptr + j));
		printk(BIOS_ERR, "\n");
	}
}

static void print_regs(uint32_t *regs)
{
	int i;

	for (i = 0; i < 16; i++) {
		if (i == 15)
			printk(BIOS_ERR, "PC");
		else if (i == 14)
			printk(BIOS_ERR, "LR");
		else if (i == 13)
			printk(BIOS_ERR, "SP");
		else if (i == 12)
			printk(BIOS_ERR, "IP");
		else
			printk(BIOS_ERR, "R%d", i);
		printk(BIOS_ERR, " = 0x%08x\n", regs[i]);
	}
}

void exception_undefined_instruction(uint32_t *regs)
{
	printk(BIOS_ERR, "exception _undefined_instruction\n");
	regs[15] -= 2;	/* CAREFUL: specific to thumb mode (otherwise 4)! */
	print_regs(regs);
	dump_stack(regs[13], 512);
	die("exception");
}

void exception_software_interrupt(uint32_t *regs)
{
	printk(BIOS_ERR, "exception _software_interrupt\n");
	print_regs(regs);
	dump_stack(regs[13], 512);
	die("exception");
}

void exception_prefetch_abort(uint32_t *regs)
{
	printk(BIOS_ERR, "exception _prefetch_abort\n");
	regs[15] -= 4;
	print_regs(regs);
	printk(BIOS_ERR, "IFAR = %#.8x\n", read_ifar());
	printk(BIOS_ERR, "IFSR = %#.8x\n", read_ifsr());
	printk(BIOS_ERR, "AIFSR = %#.8x\n", read_aifsr());
	dump_stack(regs[13], 512);
	die("exception");
}

void exception_data_abort(uint32_t *regs)
{
	printk(BIOS_ERR, "exception _data_abort\n");
	regs[15] -= 8;
	print_regs(regs);
	printk(BIOS_ERR, "DFAR = %#.8x\n", read_dfar());
	printk(BIOS_ERR, "DFSR = %#.8x\n", read_dfsr());
	printk(BIOS_ERR, "ADFSR = %#.8x\n", read_adfsr());
	dump_stack(regs[13], 512);
	die("exception");
}

void exception_not_used(uint32_t *regs)
{
	printk(BIOS_ERR, "exception _not_used\n");
	print_regs(regs);
	dump_stack(regs[13], 512);
	die("exception");
}

void exception_irq(uint32_t *regs)
{
	printk(BIOS_ERR, "exception _irq\n");
	regs[15] -= 4;
	print_regs(regs);
	dump_stack(regs[13], 512);
	die("exception");
}

void exception_fiq(uint32_t *regs)
{
	printk(BIOS_ERR, "exception _fiq\n");
	regs[15] -= 4;
	print_regs(regs);
	dump_stack(regs[13], 512);
	die("exception");
}

void exception_init(void)
{
	uint32_t sctlr = read_sctlr();
	/* Handle exceptions in ARM mode. */
	sctlr &= ~SCTLR_TE;
	/* Set V=0 in SCTLR so VBAR points to the exception vector table. */
	sctlr &= ~SCTLR_V;
	/* Enforce alignment temporarily. */
	write_sctlr(sctlr);

	extern uint32_t exception_table[];
	set_vbar((uintptr_t)exception_table);
	exception_stack_end = exception_stack + sizeof(exception_stack);

	printk(BIOS_DEBUG, "Exception handlers installed.\n");
}
