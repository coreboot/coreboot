/*
 * This file is part of the coreboot project.
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

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <program_loading.h>
#include <reset.h>
#include <rmodule.h>
#include <romstage_handoff.h>
#include <stage_cache.h>
#include <timestamp.h>
#include <security/vboot/vboot_common.h>

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

	/*
	 * Use default postcar stack size of 4 KiB. This value should
	 * not be decreased, because if mainboards use vboot, 1 KiB will
	 * not be enough anymore.
	 */

	if (stack_size == 0)
		stack_size = 4 * KiB;

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
	if (!CONFIG(BOOT_DEVICE_MEMORY_MAPPED))
		return;
	postcar_frame_add_mtrr(pcf, CACHE_ROM_BASE, CACHE_ROM_SIZE, type);
}

void postcar_frame_common_mtrrs(struct postcar_frame *pcf)
{
	if (pcf->skip_common_mtrr)
		return;

	/* Cache the ROM as WP just below 4GiB. */
	postcar_frame_add_romcache(pcf, MTRR_TYPE_WRPROT);
}

/* prepare_and_run_postcar() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use. */
void prepare_and_run_postcar(struct postcar_frame *pcf)
{
	if (postcar_frame_init(pcf, 0))
		die("Unable to initialize postcar frame.\n");

	fill_postcar_frame(pcf);

	postcar_frame_common_mtrrs(pcf);

	run_postcar_phase(pcf);
	/* We do not return here. */
}

static void postcar_commit_mtrrs(struct postcar_frame *pcf)
{
	/*
	 * Place the number of used variable MTRRs on stack then max number
	 * of variable MTRRs supported in the system.
	 */
	stack_push(pcf, pcf->num_var_mtrrs);
	stack_push(pcf, pcf->max_var_mtrrs);
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

	vboot_run_logic();

	if (prog_locate(prog))
		die_with_post_code(POST_INVALID_ROM,
				   "Failed to locate after CAR program.\n");
	if (rmodule_stage_load(&rsl))
		die_with_post_code(POST_INVALID_ROM,
				   "Failed to load after CAR program.\n");

	/* Set the stack pointer within parameters of the program loaded. */
	if (rsl.params == NULL)
		die_with_post_code(POST_INVALID_ROM,
				   "No parameters found in after CAR program.\n");

	finalize_load(rsl.params, pcf->stack);

	stage_cache_add(STAGE_POSTCAR, prog);
}

/*
 * Cache the TSEG region at the top of ram. This region is
 * not restricted to SMM mode until SMM has been relocated.
 * By setting the region to cacheable it provides faster access
 * when relocating the SMM handler as well as using the TSEG
 * region for other purposes.
 */
void postcar_enable_tseg_cache(struct postcar_frame *pcf)
{
	uintptr_t smm_base;
	size_t smm_size;

	smm_region(&smm_base, &smm_size);
	postcar_frame_add_mtrr(pcf, smm_base, smm_size,
				MTRR_TYPE_WRBACK);
}

static void postcar_cache_invalid(void)
{
	printk(BIOS_ERR, "postcar cache invalid.\n");
	board_reset();
}

void run_postcar_phase(struct postcar_frame *pcf)
{
	struct prog prog =
		PROG_INIT(PROG_POSTCAR, CONFIG_CBFS_PREFIX "/postcar");

	postcar_commit_mtrrs(pcf);

	if (!CONFIG(NO_STAGE_CACHE) &&
				romstage_handoff_is_resume()) {
		stage_cache_load_stage(STAGE_POSTCAR, &prog);
		/* This is here to allow platforms to pass different stack
		   parameters between S3 resume and normal boot. On the
		   platforms where the values are the same it's a nop. */
		finalize_load(prog.arg, pcf->stack);

		if (prog_entry(&prog) == NULL)
			postcar_cache_invalid();
	} else
		load_postcar_cbfs(&prog, pcf);

	/* As postcar exist, it's end of romstage here */
	timestamp_add_now(TS_END_ROMSTAGE);

	console_time_report();

	prog_set_arg(&prog, cbmem_top());

	prog_run(&prog);
}
