/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arch/cpu.h>
#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/fsp.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <program_loading.h>
#include <string.h>
#include <timestamp.h>

struct fsp_header fsps_hdr;

typedef asmlinkage enum fsp_status (*fsp_silicon_init_fn)
				   (void *silicon_upd);

static enum fsp_status do_silicon_init(struct fsp_header *hdr)
{
	struct FSPS_UPD upd, *supd;
	fsp_silicon_init_fn silicon_init;
	enum fsp_status status;

	supd = (struct FSPS_UPD *) (hdr->cfg_region_offset + hdr->image_base);

	if (supd->FspUpdHeader.Signature != FSPS_UPD_SIGNATURE) {
		printk(BIOS_ERR, "Invalid FSPS signature\n");
		return FSP_INCOMPATIBLE_VERSION;
	}

	memcpy(&upd, supd, sizeof(upd));

	/* Give SoC/mainboard a chance to populate entries */
	platform_fsp_silicon_init_params_cb(&upd);

	timestamp_add_now(TS_FSP_SILICON_INIT_START);
	post_code(POST_FSP_SILICON_INIT);
	silicon_init = (void *) (hdr->image_base +
				 hdr->silicon_init_entry_offset);
	status = silicon_init(&upd);
	timestamp_add_now(TS_FSP_SILICON_INIT_END);
	post_code(POST_FSP_SILICON_INIT);

	printk(BIOS_DEBUG, "FspSiliconInit returned 0x%08x\n", status);

	/* Handle any resets requested by FSPS. */
	fsp_handle_reset(status);

	return status;
}

enum fsp_status fsp_silicon_init(void)
{
	struct fsp_header *hdr = &fsps_hdr;
	struct cbfsf file_desc;
	struct region_device rdev;
	const char *name = CONFIG_FSP_S_CBFS;
	void *dest;
	size_t size;

	if (cbfs_boot_locate(&file_desc, name, NULL)) {
		printk(BIOS_ERR, "Could not locate %s in CBFS\n", name);
		return FSP_NOT_FOUND;
	}

	cbfs_file_data(&rdev, &file_desc);

	/* Load and relocate into CBMEM. */
	size = region_device_sz(&rdev);
	dest = cbmem_add(CBMEM_ID_REFCODE, size);

	if (dest == NULL) {
		printk(BIOS_ERR, "Could not add FSPS to CBMEM.\n");
		return FSP_NOT_FOUND;
	}

	if (rdev_readat(&rdev, dest, 0, size) < 0)
		return FSP_NOT_FOUND;

	if (fsp_component_relocate((uintptr_t)dest, dest, size) < 0) {
		printk(BIOS_ERR, "Unable to relocate FSPS.\n");
		return FSP_NOT_FOUND;
	}

	/* Create new region device in memory after relocation. */
	rdev_chain(&rdev, &addrspace_32bit.rdev, (uintptr_t)dest, size);

	if (fsp_validate_component(hdr, &rdev) != CB_SUCCESS)
		return FSP_NOT_FOUND;

	/* Signal that FSP component has been loaded. */
	prog_segment_loaded(hdr->image_base, hdr->image_size, SEG_FINAL);

	return do_silicon_init(hdr);
}
