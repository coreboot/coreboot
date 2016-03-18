/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <arch/cpu.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <program_loading.h>
#include <rmodule.h>

static inline void stack_push(struct postcar_frame *pcf, uint32_t val)
{
	uint32_t *ptr;

	pcf->stack -= sizeof(val);
	ptr = (void *)pcf->stack;
	*ptr = val;
}

int postcar_frame_init(struct postcar_frame *pcf, size_t stack_size)
{
	void *stack;
	msr_t msr;

	msr = rdmsr(MTRR_CAP_MSR);

	stack = cbmem_add(CBMEM_ID_ROMSTAGE_RAM_STACK, stack_size);
	if (stack == NULL) {
		printk(BIOS_ERR, "Couldn't add %zd byte stack in cbmem.\n",
			stack_size);
		return -1;
	}

	pcf->stack = (uintptr_t)stack;
	pcf->stack += stack_size;

	pcf->upper_mask = (1 << (cpu_phys_address_size() - 32)) - 1;

	pcf->max_var_mttrs = msr.lo & MTRR_CAP_VCNT;

	pcf->num_var_mttrs = 0;

	return 0;
}

void postcar_frame_add_mtrr(struct postcar_frame *pcf,
				uintptr_t addr, size_t size, int type)
{
	size_t align;

	if (pcf->num_var_mttrs >= pcf->max_var_mttrs) {
		printk(BIOS_ERR, "No more variable MTRRs: %d\n",
			pcf->max_var_mttrs);
		return;
	}

	/* Determine address alignment by lowest bit set in address. */
	align = addr & (addr ^ (addr - 1));

	if (align < size) {
		printk(BIOS_ERR, "Address (%lx) alignment (%zx) < size (%zx)\n",
			addr, align, size);
		size = align;
	}

	/* Push MTRR mask then base -- upper 32-bits then lower 32-bits. */
	stack_push(pcf, pcf->upper_mask);
	stack_push(pcf, ~(size - 1) | MTRR_PHYS_MASK_VALID);
	stack_push(pcf, 0);
	stack_push(pcf, addr | type);
	pcf->num_var_mttrs++;
}

void run_postcar_phase(struct postcar_frame *pcf)
{
	struct prog prog =
		PROG_INIT(PROG_UNKNOWN, CONFIG_CBFS_PREFIX "/postcar");
	struct rmod_stage_load rsl = {
		.cbmem_id = CBMEM_ID_AFTER_CAR,
		.prog = &prog,
	};

	/*
	 * Place the number of used variable MTRRs on stack then max number
	 * of variable MTRRs supported in the system.
	 */
	stack_push(pcf, pcf->num_var_mttrs);
	stack_push(pcf, pcf->max_var_mttrs);

	if (prog_locate(&prog))
		die("Failed to locate after CAR program.\n");
	if (rmodule_stage_load(&rsl))
		die("Failed to load after CAR program.\n");

	/* Set the stack pointer within parameters of the program loaded. */
	if (rsl.params == NULL)
		die("No parameters found in after CAR program.\n");

	*(uintptr_t *)rsl.params = pcf->stack;

	prog_run(&prog);
}
