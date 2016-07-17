/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arch/io.h>
#include <arch/cpu.h>
#include <arch/symbols.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memrange.h>
#include <reset.h>
#include <romstage_handoff.h>
#include <soc/intel/common/mrc_cache.h>
#include <string.h>
#include <timestamp.h>

typedef asmlinkage enum fsp_status (*fsp_memory_init_fn)
				   (void *raminit_upd, void **hob_list);

static enum fsp_status do_fsp_post_memory_init(void *hob_list_ptr, bool s3wake)
{
	struct range_entry fsp_mem;
	size_t  mrc_data_size;
	const void *mrc_data;
	struct romstage_handoff *handoff;

	fsp_find_reserved_memory(&fsp_mem, hob_list_ptr);

	/* initialize cbmem by adding FSP reserved memory first thing */
	if (!s3wake) {
		cbmem_initialize_empty_id_size(CBMEM_ID_FSP_RESERVED_MEMORY,
			range_entry_size(&fsp_mem));
	} else if (cbmem_initialize_id_size(CBMEM_ID_FSP_RESERVED_MEMORY,
				range_entry_size(&fsp_mem))) {
		if (IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)) {
			printk(BIOS_DEBUG, "Failed to recover CBMEM in S3 resume.\n");
			/* Failed S3 resume, reset to come up cleanly */
			hard_reset();
		}
	}

	/* make sure FSP memory is reserved in cbmem */
	if (range_entry_base(&fsp_mem) !=
		(uintptr_t)cbmem_find(CBMEM_ID_FSP_RESERVED_MEMORY))
		die("Failed to accommodate FSP reserved memory request");

	/* Now that CBMEM is up, save the list so ramstage can use it */
	fsp_save_hob_list(hob_list_ptr);

	/* Save MRC Data to CBMEM */
	if (IS_ENABLED(CONFIG_CACHE_MRC_SETTINGS) && !s3wake) {
		mrc_data = fsp_find_nv_storage_data(&mrc_data_size);
		if (mrc_data && mrc_cache_stash_data(mrc_data, mrc_data_size) < 0)
			printk(BIOS_ERR, "Failed to stash MRC data\n");
	}

	/* Create romstage handof information */
	handoff = romstage_handoff_find_or_add();
	if (handoff != NULL)
		handoff->s3_resume = s3wake;
	else
		printk(BIOS_DEBUG, "Romstage handoff structure not added!\n");

	return FSP_SUCCESS;
}

static void fsp_fill_common_arch_params(struct FSPM_ARCH_UPD *arch_upd,
					bool s3wake)
{
	const struct mrc_saved_data *mrc_cache;

	/*
	 * FSPM_UPD passed here is populated with default values provided by
	 * the blob itself. We let FSPM use top of CAR region of the size it
	 * requests.
	 * TODO: add checks to avoid overlap/conflict of CAR usage.
	 */
	arch_upd->StackBase = _car_region_end - arch_upd->StackSize;

	arch_upd->BootMode = FSP_BOOT_WITH_FULL_CONFIGURATION;

	if (IS_ENABLED(CONFIG_CACHE_MRC_SETTINGS)) {
		if (!mrc_cache_get_current_with_version(&mrc_cache, 0)) {
			/* MRC cache found */
			arch_upd->NvsBufferPtr = (void *)mrc_cache->data;
			arch_upd->BootMode = s3wake ?
				FSP_BOOT_ON_S3_RESUME:
				FSP_BOOT_ASSUMING_NO_CONFIGURATION_CHANGES;
			printk(BIOS_DEBUG, "MRC cache found, size %x bootmode:%d\n",
						mrc_cache->size, arch_upd->BootMode);
		} else
			printk(BIOS_DEBUG, "MRC cache was not found\n");
	}
}

static enum fsp_status do_fsp_memory_init(struct fsp_header *hdr, bool s3wake)
{
	enum fsp_status status;
	fsp_memory_init_fn fsp_raminit;
	struct FSPM_UPD fspm_upd, *upd;
	void *hob_list_ptr;

	post_code(0x34);

	upd = (struct FSPM_UPD *)(hdr->cfg_region_offset + hdr->image_base);

	if (upd->FspUpdHeader.Signature != FSPM_UPD_SIGNATURE) {
		printk(BIOS_ERR, "Invalid FSPM signature\n");
		return FSP_INCOMPATIBLE_VERSION;
	}

	/* Copy the default values from the UPD area */
	memcpy(&fspm_upd, upd, sizeof(fspm_upd));

	/* Reserve enough memory under TOLUD to save CBMEM header */
	fspm_upd.FspmArchUpd.BootLoaderTolumSize = cbmem_overhead_size();

	/* Fill common settings on behalf of chipset. */
	fsp_fill_common_arch_params(&fspm_upd.FspmArchUpd, s3wake);

	/* Give SoC and mainboard a chance to update the UPD */
	platform_fsp_memory_init_params_cb(&fspm_upd);

	/* Call FspMemoryInit */
	fsp_raminit = (void *)(hdr->image_base + hdr->memory_init_entry_offset);
	printk(BIOS_DEBUG, "Calling FspMemoryInit: 0x%p\n", fsp_raminit);
	printk(BIOS_SPEW, "\t%p: raminit_upd\n", &fspm_upd);
	printk(BIOS_SPEW, "\t%p: hob_list ptr\n", &hob_list_ptr);

	post_code(POST_FSP_MEMORY_INIT);
	timestamp_add_now(TS_FSP_MEMORY_INIT_START);
	status = fsp_raminit(&fspm_upd, &hob_list_ptr);
	post_code(POST_FSP_MEMORY_INIT);
	timestamp_add_now(TS_FSP_MEMORY_INIT_END);

	printk(BIOS_DEBUG, "FspMemoryInit returned 0x%08x\n", status);

	/* TODO: Is this the only thing that can happen? */
	if (status != FSP_SUCCESS)
		return status;

	return do_fsp_post_memory_init(hob_list_ptr, s3wake);
}

enum fsp_status fsp_memory_init(struct range_entry *range, bool s3wake)
{
	struct fsp_header hdr;

	if (fsp_load_binary(&hdr, CONFIG_FSP_M_CBFS, range) != CB_SUCCESS)
		return FSP_NOT_FOUND;

	return do_fsp_memory_init(&hdr, s3wake);
}
