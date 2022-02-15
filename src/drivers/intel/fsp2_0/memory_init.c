/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <security/vboot/antirollback.h>
#include <arch/symbols.h>
#include <assert.h>
#include <bootmode.h>
#include <cbfs.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <elog.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memrange.h>
#include <mrc_cache.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <string.h>
#include <symbols.h>
#include <timestamp.h>
#include <security/vboot/vboot_common.h>
#include <security/tpm/tspi.h>
#include <vb2_api.h>
#include <types.h>
#include <mode_switch.h>

static uint8_t temp_ram[CONFIG_FSP_TEMP_RAM_SIZE] __aligned(sizeof(uint64_t));

static void save_memory_training_data(bool s3wake, uint32_t fsp_version)
{
	size_t  mrc_data_size;
	const void *mrc_data;

	if (!CONFIG(CACHE_MRC_SETTINGS) || s3wake)
		return;

	mrc_data = fsp_find_nv_storage_data(&mrc_data_size);
	if (!mrc_data) {
		printk(BIOS_ERR, "FSP_NON_VOLATILE_STORAGE_HOB missing!\n");
		return;
	}

	/*
	 * Save MRC Data to CBMEM. By always saving the data this forces
	 * a retrain after a trip through Chrome OS recovery path. The
	 * code which saves the data to flash doesn't write if the latest
	 * training data matches this one.
	 */
	if (mrc_cache_stash_data(MRC_TRAINING_DATA, fsp_version, mrc_data,
				mrc_data_size) < 0)
		printk(BIOS_ERR, "Failed to stash MRC data\n");
}

static void do_fsp_post_memory_init(bool s3wake, uint32_t fsp_version)
{
	struct range_entry fsp_mem;

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

	save_memory_training_data(s3wake, fsp_version);

	/* Create romstage handof information */
	romstage_handoff_init(s3wake);
}

static void fsp_fill_mrc_cache(FSPM_ARCH_UPD *arch_upd, uint32_t fsp_version)
{
	void *data;
	size_t mrc_size;

	arch_upd->NvsBufferPtr = 0;

	if (!CONFIG(CACHE_MRC_SETTINGS))
		return;

	/* Assume boot device is memory mapped. */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	data = mrc_cache_current_mmap_leak(MRC_TRAINING_DATA, fsp_version,
					   &mrc_size);
	if (data == NULL)
		return;

	/* MRC cache found */
	arch_upd->NvsBufferPtr = (uintptr_t)data;

	printk(BIOS_SPEW, "MRC cache found, size %zx\n", mrc_size);
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

static enum cb_err setup_fsp_stack_frame(FSPM_ARCH_UPD *arch_upd,
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

static enum cb_err fsp_fill_common_arch_params(FSPM_ARCH_UPD *arch_upd,
					bool s3wake, uint32_t fsp_version,
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
	if (CONFIG(FSP_USES_CB_STACK) || !ENV_CACHE_AS_RAM) {
		arch_upd->StackBase = (uintptr_t)temp_ram;
		arch_upd->StackSize = sizeof(temp_ram);
	} else if (setup_fsp_stack_frame(arch_upd, memmap)) {
		return CB_ERR;
	}

	fsp_fill_mrc_cache(arch_upd, fsp_version);

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

	printk(BIOS_SPEW, "bootmode is set to: %d\n", arch_upd->BootMode);

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

static void do_fsp_memory_init(const struct fspm_context *context, bool s3wake)
{
	uint32_t status;
	fsp_memory_init_fn fsp_raminit;
	FSPM_UPD fspm_upd, *upd;
	FSPM_ARCH_UPD *arch_upd;
	uint32_t fsp_version;
	const struct fsp_header *hdr = &context->header;
	const struct memranges *memmap = &context->memmap;

	post_code(POST_MEM_PREINIT_PREP_START);

	fsp_version = fsp_memory_settings_version(hdr);

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
	if (fsp_fill_common_arch_params(arch_upd, s3wake, fsp_version,
					memmap) != CB_SUCCESS)
		die_with_post_code(POST_INVALID_VENDOR_BINARY,
			"FSPM_ARCH_UPD not found!\n");

	/* Give SoC and mainboard a chance to update the UPD */
	platform_fsp_memory_init_params_cb(&fspm_upd, fsp_version);

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

	post_code(POST_MEM_PREINIT_PREP_END);

	/* Call FspMemoryInit */
	fsp_raminit = (void *)(uintptr_t)(hdr->image_base + hdr->fsp_memory_init_entry_offset);
	fsp_debug_before_memory_init(fsp_raminit, upd, &fspm_upd);

	post_code(POST_FSP_MEMORY_INIT);
	timestamp_add_now(TS_FSP_MEMORY_INIT_START);
	if (ENV_X86_64 && CONFIG(PLATFORM_USES_FSP2_X86_32))
		status = protected_mode_call_2arg(fsp_raminit,
						  (uintptr_t)&fspm_upd,
						  (uintptr_t)fsp_get_hob_list_ptr());
	else
		status = fsp_raminit(&fspm_upd, fsp_get_hob_list_ptr());

	post_code(POST_FSP_MEMORY_EXIT);
	timestamp_add_now(TS_FSP_MEMORY_INIT_END);

	/* Handle any errors returned by FspMemoryInit */
	fsp_handle_reset(status);
	if (status != FSP_SUCCESS) {
		die_with_post_code(POST_RAM_FAILURE,
			"FspMemoryInit returned with error 0x%08x!\n", status);
	}

	do_fsp_post_memory_init(s3wake, fsp_version);

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

	printk(BIOS_DEBUG, "Preloading %s\n", CONFIG_FSP_M_CBFS);
	cbfs_preload(CONFIG_FSP_M_CBFS);
}

void fsp_memory_init(bool s3wake)
{
	struct range_entry prog_ranges[2];
	struct fspm_context context;
	struct fsp_load_descriptor fspld = {
		.fsp_prog = PROG_INIT(PROG_REFCODE, CONFIG_FSP_M_CBFS),
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
