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
#include <security/vboot/vboot_common.h>
#include <stage_cache.h>
#include <timestamp.h>
#include <types.h>

static size_t var_mtrr_ctx_size(void)
{
	int mtrr_count = get_var_mtrr_count();
	return sizeof(struct var_mtrr_context) + mtrr_count * 2 * sizeof(msr_t);
}

static enum cb_err postcar_frame_init(struct postcar_frame *pcf)
{
	memset(pcf, 0, sizeof(*pcf));

	struct var_mtrr_context *ctx;

	ctx = cbmem_add(CBMEM_ID_ROMSTAGE_RAM_STACK, var_mtrr_ctx_size());
	if (ctx == NULL) {
		printk(BIOS_ERR, "Couldn't add var_mtrr_ctx setup in cbmem.\n");
		return CB_ERR;
	}

	pcf->mtrr = ctx;
	var_mtrr_context_init(pcf->mtrr);

	return CB_SUCCESS;
}

void postcar_frame_add_mtrr(struct postcar_frame *pcf,
				uintptr_t addr, size_t size, int type)
{
	var_mtrr_set(pcf->mtrr, addr, size, type);
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

static void run_postcar_phase(struct postcar_frame *pcf);

/* prepare_and_run_postcar() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use. */
void __noreturn prepare_and_run_postcar(void)
{
	struct postcar_frame pcf;

	if (postcar_frame_init(&pcf))
		die("Unable to initialize postcar frame.\n");

	fill_postcar_frame(&pcf);

	postcar_frame_common_mtrrs(&pcf);

	run_postcar_phase(&pcf);
	/* We do not return here. */
	die("Failed to load postcar\n!");
}

static void finalize_load(uintptr_t *reloc_params, uintptr_t mtrr_frame_ptr)
{
	*reloc_params = mtrr_frame_ptr;
	/*
	 * Signal to rest of system that another update was made to the
	 * postcar program prior to running it.
	 */
	prog_segment_loaded((uintptr_t)reloc_params, sizeof(uintptr_t), SEG_FINAL);
	prog_segment_loaded((uintptr_t)mtrr_frame_ptr, var_mtrr_ctx_size(), SEG_FINAL);
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

	finalize_load(rsl.params, (uintptr_t)pcf->mtrr);

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

static void run_postcar_phase(struct postcar_frame *pcf)
{
	struct prog prog =
		PROG_INIT(PROG_POSTCAR, CONFIG_CBFS_PREFIX "/postcar");

	if (resume_from_stage_cache()) {
		stage_cache_load_stage(STAGE_POSTCAR, &prog);
		/* This is here to allow platforms to pass different stack
		   parameters between S3 resume and normal boot. On the
		   platforms where the values are the same it's a nop. */
		finalize_load(prog.arg, (uintptr_t)pcf->mtrr);

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
