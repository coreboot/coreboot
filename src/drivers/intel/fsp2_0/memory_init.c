/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/null_breakpoint.h>
#include <arch/stack_canary_breakpoint.h>
#include <arch/symbols.h>
#include <assert.h>
#include <cbfs.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <elog.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memrange.h>
#include <mode_switch.h>
#include <mrc_cache.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <security/tpm/tspi.h>
#include <security/vboot/antirollback.h>
#include <security/vboot/vboot_common.h>
#include <string.h>
#include <symbols.h>
#include <timestamp.h>
#include <types.h>
#include <vb2_api.h>

#if CONFIG(SOC_INTEL_COMMON_BASECODE_RAMTOP)
#include <intelbasecode/ramtop.h>
#endif

/* Callbacks for SoC/Mainboard specific overrides */
void __weak platform_fsp_memory_multi_phase_init_cb(uint32_t phase_index)
{
	/* Leave for the SoC/Mainboard to implement if necessary. */
}

static uint8_t temp_ram[CONFIG_FSP_TEMP_RAM_SIZE] __aligned(16);

/*
 * Helper function to store the MRC cache version into CBMEM
 *
 * ramstage uses either the MRC version or FSP-M version (depending on the config)
 * when updating the MRC cache
 */
static void do_cbmem_version_entry(uint32_t cbmem_id, uint32_t version)
{
	uint32_t *cbmem_version_entry = cbmem_add(cbmem_id, sizeof(version));
	if (!cbmem_version_entry) {
		printk(BIOS_ERR, "Failed to add %s version to cbmem.\n",
				CONFIG(MRC_CACHE_USING_MRC_VERSION) ? "MRC" : "FSP-M");
		return;
	}
	*cbmem_version_entry = version;
}

static void do_fsp_post_memory_init(bool s3wake, uint32_t version)
{
	struct range_entry fsp_mem;
	uint32_t cbmem_id = CONFIG(MRC_CACHE_USING_MRC_VERSION) ? CBMEM_ID_MRC_VERSION :
					 CBMEM_ID_FSPM_VERSION;

	fsp_find_reserved_memory(&fsp_mem);

	/* initialize cbmem by adding FSP reserved memory first thing */
	if (!s3wake) {
		cbmem_initialize_empty_id_size(CBMEM_ID_FSP_RESERVED_MEMORY,
			range_entry_size(&fsp_mem));
	} else if (cbmem_initialize_id_size(CBMEM_ID_FSP_RESERVED_MEMORY,
				range_entry_size(&fsp_mem))) {
		if (CONFIG(HAVE_ACPI_RESUME)) {
			printk(BIOS_ERR, "Failed to recover CBMEM in S3 resume.\n");
			/* Failed S3 resume, reset to come up cleanly */
			/* FIXME: A "system" reset is likely enough: */
			full_reset();
		}
	}

	/* make sure FSP memory is reserved in cbmem */
	if (range_entry_base(&fsp_mem) !=
		(uintptr_t)cbmem_find(CBMEM_ID_FSP_RESERVED_MEMORY))
		die("Failed to accommodate FSP reserved memory request!\n");

	if (CONFIG(CACHE_MRC_SETTINGS) && !s3wake) {
		do_cbmem_version_entry(cbmem_id, version);
		if (!CONFIG(FSP_NVS_DATA_POST_SILICON_INIT))
			save_memory_training_data();
	}

	/* Create romstage handoff information */
	romstage_handoff_init(s3wake);
}

static void fsp_fill_mrc_cache(FSPM_ARCHx_UPD *arch_upd, uint32_t version)
{
	void *data;
	size_t mrc_size;

	arch_upd->NvsBufferPtr = 0;

	if (!CONFIG(CACHE_MRC_SETTINGS))
		return;

	/* Assume boot device is memory mapped. */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	data = mrc_cache_current_mmap_leak(MRC_TRAINING_DATA, version,
					   &mrc_size);
	if (data == NULL)
		return;

	/* MRC cache found */
	arch_upd->NvsBufferPtr = (uintptr_t)data;

	printk(BIOS_SPEW, "MRC cache found, size %zu bytes\n", mrc_size);
}

static enum cb_err check_region_overlap(const struct memranges *ranges,
					const char *description,
					uintptr_t begin, uintptr_t end)
{
	const struct range_entry *r;

	memranges_each_entry(r, ranges) {
		if (end <= range_entry_base(r))
			continue;
		if (begin >= range_entry_end(r))
			continue;
		printk(BIOS_CRIT, "'%s' overlaps currently running program: "
			"[%p, %p)\n", description, (void *)begin, (void *)end);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static enum cb_err setup_fsp_stack_frame(FSPM_ARCHx_UPD *arch_upd,
		const struct memranges *memmap)
{
	uintptr_t stack_begin;
	uintptr_t stack_end;

	/*
	 * FSPM_UPD passed here is populated with default values
	 * provided by the blob itself. We let FSPM use top of CAR
	 * region of the size it requests.
	 */
	stack_end = (uintptr_t)_car_region_end;
	stack_begin = stack_end - arch_upd->StackSize;
	if (check_region_overlap(memmap, "FSPM stack", stack_begin,
				stack_end) != CB_SUCCESS)
		return CB_ERR;

	arch_upd->StackBase = stack_begin;
	return CB_SUCCESS;
}

static enum cb_err fsp_fill_common_arch_params(FSPM_ARCHx_UPD *arch_upd,
					bool s3wake, uint32_t version,
					const struct memranges *memmap)
{
	/*
	 * FSP 2.1 version would use same stack as coreboot instead of
	 * setting up separate stack frame. FSP 2.1 would not relocate stack
	 * top and does not reinitialize stack pointer. The parameters passed
	 * as StackBase and StackSize are actually for temporary RAM and HOBs
	 * and are not related to FSP stack at all.
	 * Non-CAR FSP 2.0 platforms pass a DRAM location for the FSP stack.
	 */
	static const char * const fsp_bootmode_strings[] = {
		[FSP_BOOT_WITH_FULL_CONFIGURATION] = "boot with full config",
		[FSP_BOOT_WITH_MINIMAL_CONFIGURATION] = "boot with minimal config",
		[FSP_BOOT_ASSUMING_NO_CONFIGURATION_CHANGES] = "boot assuming no config change",
		[FSP_BOOT_ON_S4_RESUME] = "boot on s4 resume",
		[FSP_BOOT_ON_S3_RESUME] = "boot on s3 resume",
		[FSP_BOOT_ON_FLASH_UPDATE] = "boot on flash update",
		[FSP_BOOT_IN_RECOVERY_MODE] = "boot in recovery mode",
	};

	if (CONFIG(FSP_USES_CB_STACK) && ENV_RAMINIT
	    && CONFIG(FSP_SPEC_VIOLATION_XEON_SP_HEAP_WORKAROUND)) {
		DECLARE_REGION(fspm_heap);
		arch_upd->StackBase = (uintptr_t)_fspm_heap;
		arch_upd->StackSize = (size_t)REGION_SIZE(fspm_heap);
	} else if (CONFIG(FSP_USES_CB_STACK) || !ENV_CACHE_AS_RAM) {
		arch_upd->StackBase = (uintptr_t)temp_ram;
		arch_upd->StackSize = sizeof(temp_ram);
	} else if (setup_fsp_stack_frame(arch_upd, memmap)) {
		return CB_ERR;
	}

	fsp_fill_mrc_cache(arch_upd, version);

	/* Configure bootmode */
	if (s3wake) {
		arch_upd->BootMode = FSP_BOOT_ON_S3_RESUME;
	} else {
		if (arch_upd->NvsBufferPtr)
			arch_upd->BootMode =
				FSP_BOOT_ASSUMING_NO_CONFIGURATION_CHANGES;
		else
			arch_upd->BootMode = FSP_BOOT_WITH_FULL_CONFIGURATION;
	}

	if (arch_upd->BootMode < ARRAY_SIZE(fsp_bootmode_strings) &&
		fsp_bootmode_strings[arch_upd->BootMode] != NULL)
		printk(BIOS_SPEW, "bootmode is set to: %d (%s)\n", arch_upd->BootMode,
			fsp_bootmode_strings[arch_upd->BootMode]);
	else
		printk(BIOS_SPEW, "bootmode is set to: %d (unknown mode)\n", arch_upd->BootMode);

	return CB_SUCCESS;
}

__weak
uint8_t fsp_memory_mainboard_version(void)
{
	return 0;
}

__weak
uint8_t fsp_memory_soc_version(void)
{
	return 0;
}

/*
 * Allow SoC and/or mainboard to bump the revision of the FSP setting
 * number. The FSP spec uses the low 8 bits as the build number. Take over
 * bits 3:0 for the SoC setting and bits 7:4 for the mainboard. That way
 * a tweak in the settings will bump the version used to track the cached
 * setting which triggers retraining when the FSP version hasn't changed, but
 * the SoC or mainboard settings have.
 */
static uint32_t fsp_memory_settings_version(const struct fsp_header *hdr)
{
	/* Use the full FSP version by default. */
	uint32_t ver = hdr->image_revision;

	if (!CONFIG(FSP_PLATFORM_MEMORY_SETTINGS_VERSIONS))
		return ver;

	ver &= ~0xff;
	ver |= (0xf & fsp_memory_mainboard_version()) << 4;
	ver |= (0xf & fsp_memory_soc_version()) << 0;

	return ver;
}

struct fspm_context {
	struct fsp_header header;
	struct memranges memmap;
};

/*
 * Helper function to read MRC version
 *
 * There are multiple ways to read the MRC version using
 * Intel FSP. Currently the only supported method to get the
 * MRC version is by reading the FSP_PRODUCDER_DATA_TABLES
 * from the FSP-M binary (by parsing the FSP header).
 */
static uint32_t fsp_mrc_version(const struct fsp_header *hdr)
{
	uint32_t ver = 0;
#if CONFIG(MRC_CACHE_USING_MRC_VERSION)
	void *fspm_blob_file = (void *)(uintptr_t)hdr->image_base;
	FSP_PRODUCER_DATA_TABLES *ft = fspm_blob_file + FSP_HDR_OFFSET;
	FSP_PRODUCER_DATA_TYPE2 *table2 = &ft->FspProduceDataType2;
	size_t mrc_version_size = sizeof(table2->MrcVersion);
	for (size_t i = 0; i < mrc_version_size; i++) {
		ver |= (table2->MrcVersion[i] << ((mrc_version_size - 1) - i) * 8);
	}
#endif
	return ver;
}

static void fspm_return_value_handler(const char *context, efi_return_status_t status,
		 bool die_on_error)
{
	if (status == FSP_SUCCESS)
		return;

	fsp_handle_reset(status);
	if (die_on_error)
		fsp_die_with_post_code(status, POSTCODE_RAM_FAILURE, "%s error", context);

	fsp_printk(status, BIOS_SPEW, "%s", context);
}

static void fspm_multi_phase_init(const struct fsp_header *hdr)
{
	efi_return_status_t status;
	fsp_multi_phase_init_fn fsp_multi_phase_init;
	struct fsp_multi_phase_params multi_phase_params;
	struct fsp_multi_phase_get_number_of_phases_params multi_phase_get_number;

	if (!hdr->fsp_multi_phase_mem_init_entry_offset)
		return;

	fsp_multi_phase_init = (fsp_multi_phase_init_fn)(uintptr_t)
		(hdr->image_base + hdr->fsp_multi_phase_mem_init_entry_offset);

	post_code(POSTCODE_FSP_MULTI_PHASE_MEM_INIT_ENTRY);
	timestamp_add_now(TS_FSP_MULTI_PHASE_MEM_INIT_START);

	/* Get number of phases */
	multi_phase_params.multi_phase_action = GET_NUMBER_OF_PHASES;
	multi_phase_params.phase_index = 0;
	multi_phase_params.multi_phase_param_ptr = &multi_phase_get_number;
	status = fsp_multi_phase_init(&multi_phase_params);
	fspm_return_value_handler("FspMultiPhaseMemInit NumberOfPhases", status, false);

	/* Execute all phases */
	for (uint32_t i = 1; i <= multi_phase_get_number.number_of_phases; i++) {
		printk(BIOS_SPEW, "Executing Phase %u of FspMultiPhaseMemInit\n", i);
		/*
		 * Give SoC/mainboard a chance to perform any operation before
		 * Multi Phase Execution
		 */
		platform_fsp_memory_multi_phase_init_cb(i);

		multi_phase_params.multi_phase_action = EXECUTE_PHASE;
		multi_phase_params.phase_index = i;
		multi_phase_params.multi_phase_param_ptr = NULL;
		status = fsp_multi_phase_init(&multi_phase_params);
		fspm_return_value_handler("FspMultiPhaseMemInit Execute", status, false);
	}

	post_code(POSTCODE_FSP_MULTI_PHASE_MEM_INIT_EXIT);
	timestamp_add_now(TS_FSP_MULTI_PHASE_MEM_INIT_END);
}

static void do_fsp_memory_init(const struct fspm_context *context, bool s3wake)
{
	efi_return_status_t status;
	fsp_memory_init_fn fsp_raminit;
	FSPM_UPD fspm_upd, *upd;
	FSPM_ARCHx_UPD *arch_upd;
	uint32_t version;
	const struct fsp_header *hdr = &context->header;
	const struct memranges *memmap = &context->memmap;

	post_code(POSTCODE_MEM_PREINIT_PREP_START);

	if (CONFIG(MRC_CACHE_USING_MRC_VERSION))
		version = fsp_mrc_version(hdr);
	else
		version = fsp_memory_settings_version(hdr);

	upd = (FSPM_UPD *)(uintptr_t)(hdr->cfg_region_offset + hdr->image_base);

	/*
	 * Verify UPD region size. We don't have malloc before ramstage, so we
	 * use a static buffer for the FSP-M UPDs which is sizeof(FSPM_UPD)
	 * bytes long, since that is the value known at compile time. If
	 * hdr->cfg_region_size is bigger than that, not all UPD defaults will
	 * be copied, so it'll contain random data at the end, so we just call
	 * die() in that case. If hdr->cfg_region_size is smaller than that,
	 * there's a mismatch between the FSP and the header, but since it will
	 * copy the full UPD defaults to the buffer, we try to continue and
	 * hope that there was no incompatible change in the UPDs.
	 */
	if (hdr->cfg_region_size > sizeof(FSPM_UPD))
		die("FSP-M UPD size is larger than FSPM_UPD struct size.\n");
	if (hdr->cfg_region_size < sizeof(FSPM_UPD))
		printk(BIOS_ERR, "FSP-M UPD size is smaller than FSPM_UPD struct size. "
				"Check if the FSP binary matches the FSP headers.\n");

	fsp_verify_upd_header_signature(upd->FspUpdHeader.Signature, FSPM_UPD_SIGNATURE);

	/* Copy the default values from the UPD area */
	memcpy(&fspm_upd, upd, sizeof(fspm_upd));

	arch_upd = &fspm_upd.FspmArchUpd;

	/* Reserve enough memory under TOLUD to save CBMEM header */
	arch_upd->BootLoaderTolumSize = cbmem_overhead_size();

	/* Fill common settings on behalf of chipset. */
	if (fsp_fill_common_arch_params(arch_upd, s3wake, version,
					memmap) != CB_SUCCESS)
		die_with_post_code(POSTCODE_INVALID_VENDOR_BINARY,
			"FSPM_ARCH_UPD not found!\n");

	/* Early caching of RAMTOP region if valid mrc cache data is found */
#if (CONFIG(SOC_INTEL_COMMON_BASECODE_RAMTOP))
	if (arch_upd->NvsBufferPtr)
		early_ramtop_enable_cache_range();
#endif

	/* Give SoC and mainboard a chance to update the UPD */
	platform_fsp_memory_init_params_cb(&fspm_upd, version);

	/*
	 * For S3 resume case, if valid mrc cache data is not found or
	 * RECOVERY_MRC_CACHE hash verification fails, the S3 data
	 * pointer would be null and S3 resume fails with fsp-m
	 * returning error. Invoking a reset here saves time.
	 */
	if (s3wake && !arch_upd->NvsBufferPtr)
		/* FIXME: A "system" reset is likely enough: */
		full_reset();

	if (CONFIG(MMA))
		setup_mma(&fspm_upd.FspmConfig);

	post_code(POSTCODE_MEM_PREINIT_PREP_END);

	/* Call FspMemoryInit */
	fsp_raminit = (void *)(uintptr_t)(hdr->image_base + hdr->fsp_memory_init_entry_offset);
	fsp_debug_before_memory_init(fsp_raminit, upd, &fspm_upd);

	/* FSP disables the interrupt handler so remove debug exceptions temporarily  */
	null_breakpoint_disable();
	stack_canary_breakpoint_disable();
	post_code(POSTCODE_FSP_MEMORY_INIT);
	timestamp_add_now(TS_FSP_MEMORY_INIT_START);
	if (ENV_X86_64 && CONFIG(PLATFORM_USES_FSP2_X86_32))
		status = protected_mode_call_2arg(fsp_raminit,
						  (uintptr_t)&fspm_upd,
						  (uintptr_t)fsp_get_hob_list_ptr());
	else
		status = fsp_raminit(&fspm_upd, fsp_get_hob_list_ptr());
	null_breakpoint_init();
	stack_canary_breakpoint_init();

	post_code(POSTCODE_FSP_MEMORY_EXIT);
	timestamp_add_now(TS_FSP_MEMORY_INIT_END);

	/* Handle any errors returned by FspMemoryInit */
	fspm_return_value_handler("FspMemoryInit", status, true);

	if (CONFIG(PLATFORM_USES_FSP2_4))
		fspm_multi_phase_init(hdr);

	do_fsp_post_memory_init(s3wake, version);

	/*
	 * fsp_debug_after_memory_init() checks whether the end of the tolum
	 * region is the same as the top of cbmem, so must be called here
	 * after cbmem has been initialised in do_fsp_post_memory_init().
	 */
	fsp_debug_after_memory_init(status);
}

static void *fspm_allocator(void *arg, size_t size, const union cbfs_mdata *unused)
{
	const struct fsp_load_descriptor *fspld = arg;
	struct fspm_context *context = fspld->arg;
	struct memranges *memmap = &context->memmap;

	/* Non XIP FSP-M uses FSP-M address */
	uintptr_t fspm_begin = (uintptr_t)CONFIG_FSP_M_ADDR;
	uintptr_t fspm_end = fspm_begin + size;

	if (check_region_overlap(memmap, "FSPM", fspm_begin, fspm_end) != CB_SUCCESS)
		return NULL;

	return (void *)fspm_begin;
}

void preload_fspm(void)
{
	if (!CONFIG(CBFS_PRELOAD))
		return;

	const char *fspm_cbfs = soc_select_fsp_m_cbfs();
	printk(BIOS_DEBUG, "Preloading %s\n", fspm_cbfs);
	cbfs_preload(fspm_cbfs);
}

void fsp_memory_init(bool s3wake)
{
	struct range_entry prog_ranges[2];
	struct fspm_context context;
	const char *fspm_cbfs = soc_select_fsp_m_cbfs();
	struct fsp_load_descriptor fspld = {
		.fsp_prog = PROG_INIT(PROG_REFCODE, fspm_cbfs),
		.arg = &context,
	};
	struct fsp_header *hdr = &context.header;
	struct memranges *memmap = &context.memmap;

	/* For FSP-M XIP we leave alloc NULL to get a direct mapping to flash. */
	if (!CONFIG(FSP_M_XIP))
		fspld.alloc = fspm_allocator;

	elog_boot_notify(s3wake);

	/* Build up memory map of romstage address space including CAR. */
	memranges_init_empty(memmap, &prog_ranges[0], ARRAY_SIZE(prog_ranges));
	if (ENV_CACHE_AS_RAM)
		memranges_insert(memmap, (uintptr_t)_car_region_start,
			_car_unallocated_start - _car_region_start, 0);
	memranges_insert(memmap, (uintptr_t)_program, REGION_SIZE(program), 0);

	timestamp_add_now(TS_FSP_MEMORY_INIT_LOAD);
	if (fsp_load_component(&fspld, hdr) != CB_SUCCESS)
		die("FSPM not available or failed to load!\n");

	if (CONFIG(FSP_M_XIP) && (uintptr_t)prog_start(&fspld.fsp_prog) != hdr->image_base)
		die("FSPM XIP base does not match: %p vs %p\n",
		    (void *)(uintptr_t)hdr->image_base, prog_start(&fspld.fsp_prog));

	timestamp_add_now(TS_INITRAM_START);

	do_fsp_memory_init(&context, s3wake);

	timestamp_add_now(TS_INITRAM_END);
}
