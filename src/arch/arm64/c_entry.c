/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/lib_helpers.h>
#include <arch/mmu.h>
#include <arch/stages.h>
#include <gic.h>

#include "cpu-internal.h"

void __attribute__((weak)) arm64_soc_init(void)
{
	/* Default weak implementation does nothing. */
}

static void seed_stack(void)
{
	char *stack_begin;
	uint64_t *slot;
	int i;
	int size;

	stack_begin = cpu_get_stack();
	stack_begin -= CONFIG_STACK_SIZE;
	slot = (void *)stack_begin;

	/* Pad out 256 bytes for current usage. */
	size = CONFIG_STACK_SIZE - 256;
	size /= sizeof(*slot);
	for (i = 0; i < size; i++)
		*slot++ = 0xdeadbeefdeadbeefULL;
}

/* Set up default SCR values. */
static void el3_init(void)
{
	uint32_t scr;

	if (get_current_el() != EL3)
		return;

	scr = raw_read_scr_el3();
	/* Default to non-secure EL1 and EL0. */
	scr &= ~(SCR_NS_MASK);
	scr |= SCR_NS_ENABLE;
	/* Disable IRQ, FIQ, and external abort interrupt routing. */
	scr &= ~(SCR_IRQ_MASK | SCR_FIQ_MASK | SCR_EA_MASK);
	scr |= SCR_IRQ_DISABLE | SCR_FIQ_DISABLE | SCR_EA_DISABLE;
	/* Enable HVC */
	scr &= ~(SCR_HVC_MASK);
	scr |= SCR_HVC_ENABLE;
	/* Disable SMC */
	scr &= ~(SCR_SMC_MASK);
	scr |= SCR_SMC_DISABLE;
	/* Disable secure instruction fetches. */
	scr &= ~(SCR_SIF_MASK);
	scr |= SCR_SIF_DISABLE;
	/* All lower exception levels 64-bit by default. */
	scr &= ~(SCR_RW_MASK);
	scr |= SCR_LOWER_AARCH64;
	/* Disable secure EL1 access to secure timer. */
	scr &= ~(SCR_ST_MASK);
	scr |= SCR_ST_DISABLE;
	/* Don't trap on WFE or WFI instructions. */
	scr &= ~(SCR_TWI_MASK | SCR_TWE_MASK);
	scr |= SCR_TWI_DISABLE | SCR_TWE_DISABLE;
	raw_write_scr_el3(scr);
	isb();
}

void __attribute__((weak)) arm64_arch_timer_init(void)
{
	/* Default weak implementation does nothing. */
}

static void arm64_init(void)
{
	seed_stack();

	/* Set up default SCR values. */
	el3_init();

	/* Initialize the GIC. */
	gic_init();

	/*
	 * Disable coprocessor traps to EL3:
	 * TCPAC [20] = 0, disable traps for EL2 accesses to CPTR_EL2 or HCPTR
	 * and EL2/EL1 access to CPACR_EL1.
	 * TTA [20] = 0, disable traps for trace register access from any EL.
	 * TFP [10] = 0, disable traps for floating-point instructions from any
	 * EL.
	 */
	raw_write_cptr_el3(CPTR_EL3_TCPAC_DISABLE | CPTR_EL3_TTA_DISABLE |
			   CPTR_EL3_TFP_DISABLE);

	/*
	 * Allow FPU accesses:
	 * FPEN [21:20] = 3, disable traps for floating-point instructions from
	 * EL0/EL1.
	 * TTA [28] = 0, disable traps for trace register access from EL0/EL1.
	 */
	raw_write_cpacr_el1(CPACR_TRAP_FP_DISABLE | CPACR_TTA_DISABLE);

	/* Arch Timer init: setup cntfrq per CPU */
	arm64_arch_timer_init();

	arm64_soc_init();

	main();
}

/* This variable holds entry point for CPU starting up. */
void (*c_entry)(void) = &arm64_init;
