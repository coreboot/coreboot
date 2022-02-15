/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <program_loading.h>
#include <reset.h>
#include <rmodule.h>
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
	var_mtrr_context_init(&pcf->ctx, pcf);
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

static void postcar_var_mtrr_set(const struct var_mtrr_context *ctx,
				uintptr_t addr, size_t size,
				msr_t base, msr_t mask)
{
	struct postcar_frame *pcf = ctx->arg;

	printk(BIOS_DEBUG, "MTRR Range: Start=%lx End=%lx (Size %zx)\n",
			addr, addr + size, size);

	stack_push(pcf, mask.hi);
	stack_push(pcf, mask.lo);
	stack_push(pcf, base.hi);
	stack_push(pcf, base.lo);
}

void postcar_frame_add_mtrr(struct postcar_frame *pcf,
				uintptr_t addr, size_t size, int type)
{
	var_mtrr_set_with_cb(&pcf->ctx, addr, size, type, postcar_var_mtrr_set);
}

void postcar_frame_add_romcache(struct postcar_frame *pcf, int type)
{
	if (!CONFIG(BOOT_DEVICE_MEMORY_MAPPED))
		return;
	postcar_frame_add_mtrr(pcf, CACHE_ROM_BASE, CACHE_ROM_SIZE, type);
}

static void postcar_frame_common_mtrrs(struct postcar_frame *pcf)
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
	stack_push(pcf, pcf->ctx.used_var_mtrrs);
	stack_push(pcf, pcf->ctx.max_var_mtrrs);
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

	if (resume_from_stage_cache()) {
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
	timestamp_add_now(TS_ROMSTAGE_END);

	console_time_report();

	prog_set_arg(&prog, cbmem_top());

	prog_run(&prog);
}
