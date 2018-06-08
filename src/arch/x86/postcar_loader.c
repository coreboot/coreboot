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

static void postcar_frame_prepare(struct postcar_frame *pcf)
{
	msr_t msr;
	msr = rdmsr(MTRR_CAP_MSR);

	pcf->upper_mask = (1 << (cpu_phys_address_size() - 32)) - 1;
	pcf->max_var_mtrrs = msr.lo & MTRR_CAP_VCNT;
	pcf->num_var_mtrrs = 0;
}

int postcar_frame_init(struct postcar_frame *pcf, size_t stack_size)
{
	void *stack;

	stack = cbmem_add(CBMEM_ID_ROMSTAGE_RAM_STACK, stack_size);
	if (stack == NULL) {
		printk(BIOS_ERR, "Couldn't add %zd byte stack in cbmem.\n",
			stack_size);
		return -1;
	}

	postcar_frame_prepare(pcf);
	pcf->stack = (uintptr_t)stack;
	pcf->stack += stack_size;
	return 0;
}

/*
 * For use with LATE_CBMEM_INIT boards only, with a fixed stacktop in
 * low memory.
 */
void postcar_frame_init_lowmem(struct postcar_frame *pcf)
{
	postcar_frame_prepare(pcf);
	pcf->stack = CONFIG_RAMTOP;
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

		if (pcf->num_var_mtrrs >= pcf->max_var_mtrrs) {
			printk(BIOS_ERR, "No more variable MTRRs: %d\n",
					pcf->max_var_mtrrs);
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
		pcf->num_var_mtrrs++;

		size -= mtrr_size;
		addr += mtrr_size;
	}
}

void postcar_frame_add_romcache(struct postcar_frame *pcf, int type)
{
	if (!IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED))
		return;
	postcar_frame_add_mtrr(pcf, CACHE_ROM_BASE, CACHE_ROM_SIZE, type);
}

void *postcar_commit_mtrrs(struct postcar_frame *pcf)
{
	/*
	 * Place the number of used variable MTRRs on stack then max number
	 * of variable MTRRs supported in the system.
	 */
	stack_push(pcf, pcf->num_var_mtrrs);
	stack_push(pcf, pcf->max_var_mtrrs);
	return (void *) pcf->stack;
}

static void finalize_load(uintptr_t *stack_top_ptr, uintptr_t stack_top)
{
	*stack_top_ptr = stack_top;
	/*
	 * Signal to rest of system that another update was made to the
	 * postcar program prior to running it.
	 */
	prog_segment_loaded((uintptr_t)stack_top_ptr, sizeof(uintptr_t),
		SEG_FINAL);
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

	finalize_load(rsl.params, pcf->stack);

	if (!IS_ENABLED(CONFIG_NO_STAGE_CACHE))
		stage_cache_add(STAGE_POSTCAR, prog);
}

void run_postcar_phase(struct postcar_frame *pcf)
{
	struct prog prog =
		PROG_INIT(PROG_UNKNOWN, CONFIG_CBFS_PREFIX "/postcar");

	postcar_commit_mtrrs(pcf);

	if (!IS_ENABLED(CONFIG_NO_STAGE_CACHE) &&
				romstage_handoff_is_resume()) {
		stage_cache_load_stage(STAGE_POSTCAR, &prog);
		/* This is here to allow platforms to pass different stack
		   parameters between S3 resume and normal boot. On the
		   platforms where the values are the same it's a nop. */
		finalize_load(prog.arg, pcf->stack);
	} else
		load_postcar_cbfs(&prog, pcf);

	prog_run(&prog);
}
