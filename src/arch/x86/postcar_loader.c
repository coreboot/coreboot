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
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <program_loading.h>
#include <rmodule.h>
#include <romstage_handoff.h>
#include <stage_cache.h>

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
	/*
	 * Utilize additional MTRRs if the specified size is greater than the
	 * base address alignment.
	 */
	while (size != 0) {
		uint32_t addr_lsb;
		uint32_t size_msb;
		uint32_t mtrr_size;

		if (pcf->num_var_mttrs >= pcf->max_var_mttrs) {
			printk(BIOS_ERR, "No more variable MTRRs: %d\n",
					pcf->max_var_mttrs);
			return;
		}

		addr_lsb = fls(addr);
		size_msb = fms(size);

		/* All MTRR entries need to have their base aligned to the mask
		 * size. The maximum size is calculated by a function of the
		 * min base bit set and maximum size bit set. */
		if (addr_lsb > size_msb)
			mtrr_size = 1 << size_msb;
		else
			mtrr_size = 1 << addr_lsb;

		printk(BIOS_DEBUG, "MTRR Range: Start=%lx End=%lx (Size %x)\n",
					addr, addr + mtrr_size, mtrr_size);

		stack_push(pcf, pcf->upper_mask);
		stack_push(pcf, ~(mtrr_size - 1) | MTRR_PHYS_MASK_VALID);
		stack_push(pcf, 0);
		stack_push(pcf, addr | type);
		pcf->num_var_mttrs++;

		size -= mtrr_size;
		addr += mtrr_size;
	}
}

void *postcar_commit_mtrrs(struct postcar_frame *pcf)
{
	/*
	 * Place the number of used variable MTRRs on stack then max number
	 * of variable MTRRs supported in the system.
	 */
	stack_push(pcf, pcf->num_var_mttrs);
	stack_push(pcf, pcf->max_var_mttrs);
	return (void *) pcf->stack;
}

static void load_postcar_cbfs(struct prog *prog, struct postcar_frame *pcf)
{
	struct rmod_stage_load rsl = {
		.cbmem_id = CBMEM_ID_AFTER_CAR,
		.prog = prog,
	};

	if (prog_locate(prog))
		die("Failed to locate after CAR program.\n");
	if (rmodule_stage_load(&rsl))
		die("Failed to load after CAR program.\n");

	/* Set the stack pointer within parameters of the program loaded. */
	if (rsl.params == NULL)
		die("No parameters found in after CAR program.\n");

	*(uintptr_t *)rsl.params = pcf->stack;

	/*
	 * Signal to rest of system that another update was made to the
	 * postcar program prior to running it.
	 */
	prog_segment_loaded((uintptr_t)rsl.params, sizeof(uintptr_t),
		SEG_FINAL);

	if (!IS_ENABLED(CONFIG_NO_STAGE_CACHE))
		stage_cache_add(STAGE_POSTCAR, prog);
}

void run_postcar_phase(struct postcar_frame *pcf)
{
	struct prog prog =
		PROG_INIT(PROG_UNKNOWN, CONFIG_CBFS_PREFIX "/postcar");

	postcar_commit_mtrrs(pcf);

	if (!IS_ENABLED(CONFIG_NO_STAGE_CACHE) && romstage_handoff_is_resume())
		stage_cache_load_stage(STAGE_POSTCAR, &prog);
	else
		load_postcar_cbfs(&prog, pcf);

	prog_run(&prog);
}
