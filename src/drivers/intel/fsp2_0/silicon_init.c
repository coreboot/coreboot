/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/null_breakpoint.h>
#include <bootsplash.h>
#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/fsp.h>
#include <commonlib/stdlib.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <program_loading.h>
#include <soc/intel/common/vbt.h>
#include <stage_cache.h>
#include <string.h>
#include <timestamp.h>
#include <types.h>
#include <mode_switch.h>

struct fsp_header fsps_hdr;

struct fsp_multi_phase_get_number_of_phases_params {
	uint32_t number_of_phases;
	uint32_t phases_executed;
};

/* Callbacks for SoC/Mainboard specific overrides */
void __weak platform_fsp_multi_phase_init_cb(uint32_t phase_index)
{
	/* Leave for the SoC/Mainboard to implement if necessary. */
}

/* FSP Specification < 2.2 has only 1 stage like FspSiliconInit. FSP specification >= 2.2
 * has multiple stages as below.
 */
enum fsp_silicon_init_phases {
	FSP_SILICON_INIT_API,
	FSP_MULTI_PHASE_SI_INIT_GET_NUMBER_OF_PHASES_API,
	FSP_MULTI_PHASE_SI_INIT_EXECUTE_PHASE_API
};

static void fsps_return_value_handler(enum fsp_silicon_init_phases phases, uint32_t status)
{
	uint8_t postcode;

	/* Handle any reset request returned by FSP-S APIs */
	fsp_handle_reset(status);

	if (status == FSP_SUCCESS)
		return;
	/* Handle all other errors returned by FSP-S APIs */
	/* Assume video failure if attempted to initialize graphics */
	if (CONFIG(RUN_FSP_GOP) && vbt_get())
		postcode = POST_VIDEO_FAILURE;
	else
		postcode = POST_HW_INIT_FAILURE; /* else generic */

	switch (phases) {
	case FSP_SILICON_INIT_API:
		die_with_post_code(postcode, "FspSiliconInit returned with error 0x%08x\n",
				status);
		break;
	case FSP_MULTI_PHASE_SI_INIT_GET_NUMBER_OF_PHASES_API:
		printk(BIOS_SPEW, "FspMultiPhaseSiInit NumberOfPhases returned 0x%08x\n",
				status);
		break;
	case FSP_MULTI_PHASE_SI_INIT_EXECUTE_PHASE_API:
		printk(BIOS_SPEW, "FspMultiPhaseSiInit ExecutePhase returned 0x%08x\n",
				status);
		break;
	default:
		break;
	}
}

bool fsp_is_multi_phase_init_enabled(void)
{
	return CONFIG(FSPS_USE_MULTI_PHASE_INIT) &&
			 (fsps_hdr.fsp_multi_phase_si_init_entry_offset != 0);
}

static void fsp_fill_common_arch_params(FSPS_UPD *supd)
{
#if CONFIG(FSPS_HAS_ARCH_UPD)
	FSPS_ARCH_UPD *s_arch_cfg = &supd->FspsArchUpd;
	s_arch_cfg->EnableMultiPhaseSiliconInit = fsp_is_multi_phase_init_enabled();
#endif
}

static void do_silicon_init(struct fsp_header *hdr)
{
	FSPS_UPD *upd, *supd;
	fsp_silicon_init_fn silicon_init;
	uint32_t status;
	fsp_multi_phase_si_init_fn multi_phase_si_init;
	struct fsp_multi_phase_params multi_phase_params;
	struct fsp_multi_phase_get_number_of_phases_params multi_phase_get_number;

	supd = (FSPS_UPD *) (uintptr_t)(hdr->cfg_region_offset + hdr->image_base);

	fsp_verify_upd_header_signature(supd->FspUpdHeader.Signature, FSPS_UPD_SIGNATURE);

	/* FSPS UPD and coreboot structure sizes should match. However, enforcing the exact
	 * match mandates simultaneous updates to coreboot and FSP repos. Allow coreboot
	 * to proceed if its UPD structure is smaller than FSP one to enable staggered UPD
	 * update process on both sides. The mismatch indicates a temporary build problem,
	 * don't leave it like this as FSP default settings can be bad choices for coreboot.
	 */
	if (!hdr->cfg_region_size || hdr->cfg_region_size < sizeof(FSPS_UPD))
		die_with_post_code(POST_INVALID_VENDOR_BINARY,
			"Invalid FSPS UPD region\n");
	else if (hdr->cfg_region_size > sizeof(FSPS_UPD))
		printk(BIOS_ERR, "FSP and coreboot are out of sync! FSPS UPD size > coreboot\n");

	upd = xmalloc(hdr->cfg_region_size);

	memcpy(upd, supd, hdr->cfg_region_size);

	/* Fill common settings on behalf of chipset. */
	if (CONFIG(FSPS_HAS_ARCH_UPD))
		fsp_fill_common_arch_params(upd);
	/* Give SoC/mainboard a chance to populate entries */
	platform_fsp_silicon_init_params_cb(upd);

	/* Populate logo related entries */
	if (CONFIG(BMP_LOGO))
		soc_load_logo(upd);

	/* Call SiliconInit */
	silicon_init = (void *) (uintptr_t)(hdr->image_base +
				 hdr->fsp_silicon_init_entry_offset);
	fsp_debug_before_silicon_init(silicon_init, supd, upd);

	timestamp_add_now(TS_FSP_SILICON_INIT_START);
	post_code(POST_FSP_SILICON_INIT);

	/* FSP disables the interrupt handler so remove debug exceptions temporarily  */
	null_breakpoint_disable();
	if (ENV_X86_64 && CONFIG(PLATFORM_USES_FSP2_X86_32))
		status = protected_mode_call_1arg(silicon_init, (uintptr_t)upd);
	else
		status = silicon_init(upd);
	null_breakpoint_init();

	printk(BIOS_INFO, "FSPS returned %x\n", status);

	timestamp_add_now(TS_FSP_SILICON_INIT_END);
	post_code(POST_FSP_SILICON_EXIT);

	if (CONFIG(BMP_LOGO))
		bmp_release_logo();

	fsp_debug_after_silicon_init(status);
	fsps_return_value_handler(FSP_SILICON_INIT_API, status);

	/* Reinitialize CPUs if FSP-S has done MP Init */
	if (CONFIG(USE_INTEL_FSP_MP_INIT))
		do_mpinit_after_fsp();

	if (!CONFIG(PLATFORM_USES_FSP2_2))
		return;

	/* Check if SoC user would like to call Multi Phase Init */
	if (!fsp_is_multi_phase_init_enabled())
		return;

	/* Call MultiPhaseSiInit */
	multi_phase_si_init = (void *) (uintptr_t)(hdr->image_base +
			 hdr->fsp_multi_phase_si_init_entry_offset);

	/* Implementing multi_phase_si_init() is optional as per FSP 2.2 spec */
	if (multi_phase_si_init == NULL)
		return;

	post_code(POST_FSP_MULTI_PHASE_SI_INIT_ENTRY);
	timestamp_add_now(TS_FSP_MULTI_PHASE_SI_INIT_START);
	/* Get NumberOfPhases Value */
	multi_phase_params.multi_phase_action = GET_NUMBER_OF_PHASES;
	multi_phase_params.phase_index = 0;
	multi_phase_params.multi_phase_param_ptr = &multi_phase_get_number;
	status = multi_phase_si_init(&multi_phase_params);
	fsps_return_value_handler(FSP_MULTI_PHASE_SI_INIT_GET_NUMBER_OF_PHASES_API, status);

	/* Execute Multi Phase Execution */
	for (uint32_t i = 1; i <= multi_phase_get_number.number_of_phases; i++) {
		printk(BIOS_SPEW, "Executing Phase %u of FspMultiPhaseSiInit\n", i);
		/*
		 * Give SoC/mainboard a chance to perform any operation before
		 * Multi Phase Execution
		 */
		platform_fsp_multi_phase_init_cb(i);

		multi_phase_params.multi_phase_action = EXECUTE_PHASE;
		multi_phase_params.phase_index = i;
		multi_phase_params.multi_phase_param_ptr = NULL;
		status = multi_phase_si_init(&multi_phase_params);
		fsps_return_value_handler(FSP_MULTI_PHASE_SI_INIT_EXECUTE_PHASE_API, status);
	}
	timestamp_add_now(TS_FSP_MULTI_PHASE_SI_INIT_END);
	post_code(POST_FSP_MULTI_PHASE_SI_INIT_EXIT);
}

static void *fsps_allocator(void *arg_unused, size_t size, const union cbfs_mdata *mdata_unused)
{
	return cbmem_add(CBMEM_ID_REFCODE, size);
}

void fsps_load(void)
{
	struct fsp_load_descriptor fspld = {
		.fsp_prog = PROG_INIT(PROG_REFCODE, CONFIG_FSP_S_CBFS),
		.alloc = fsps_allocator,
	};
	struct prog *fsps = &fspld.fsp_prog;
	static int load_done;

	if (load_done)
		return;

	timestamp_add_now(TS_FSP_SILICON_INIT_LOAD);

	if (resume_from_stage_cache()) {
		printk(BIOS_DEBUG, "Loading FSPS from stage_cache\n");
		stage_cache_load_stage(STAGE_REFCODE, fsps);
		if (fsp_validate_component(&fsps_hdr, prog_start(fsps), prog_size(fsps)))
			die("On resume fsps header is invalid\n");
		load_done = 1;
		return;
	}

	if (fsp_load_component(&fspld, &fsps_hdr) != CB_SUCCESS)
		die("FSP-S failed to load\n");

	stage_cache_add(STAGE_REFCODE, fsps);

	load_done = 1;
}

void preload_fsps(void)
{
	if (!CONFIG(CBFS_PRELOAD))
		return;

	printk(BIOS_DEBUG, "Preloading %s\n", CONFIG_FSP_S_CBFS);
	cbfs_preload(CONFIG_FSP_S_CBFS);
}

void fsp_silicon_init(void)
{
	fsps_load();
	do_silicon_init(&fsps_hdr);

	if (CONFIG(DISPLAY_FSP_TIMESTAMPS))
		fsp_display_timestamp();
}

__weak void soc_load_logo(FSPS_UPD *supd) { }
