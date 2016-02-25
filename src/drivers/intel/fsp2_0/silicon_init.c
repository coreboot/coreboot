/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arch/cpu.h>
#include <cbfs.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <string.h>

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

	silicon_init = (void *) (hdr->image_base +
						hdr->silicon_init_entry_offset);

	status = silicon_init(&upd);
	printk(BIOS_DEBUG, "FspSiliconInit returned 0x%08x\n", status);
	return status;
}

enum fsp_status fsp_silicon_init(struct range_entry *range)
{
	/* Load FSP-S and save FSP header. We will need it for Notify */
	/* TODO: do not hardcode CBFS file names */
	if (fsp_load_binary(&fsps_hdr, "blobs/fsps.bin", range) != CB_SUCCESS)
		return FSP_NOT_FOUND;

	return do_silicon_init(&fsps_hdr);
}
