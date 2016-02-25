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
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memrange.h>
#include <string.h>
#include <timestamp.h>

typedef asmlinkage enum fsp_status (*fsp_memory_init_fn)
				   (void *raminit_upd, void **hob_list);

static enum fsp_status do_fsp_memory_init(void **hob_list_ptr,
					  struct fsp_header *hdr)
{
	enum fsp_status status;
	fsp_memory_init_fn fsp_raminit;
	struct FSPM_UPD fspm_upd, *upd;

	post_code(0x34);

	upd = (struct FSPM_UPD *)(hdr->cfg_region_offset + hdr->image_base);

	if (upd->FspUpdHeader.Signature != FSPM_UPD_SIGNATURE) {
		printk(BIOS_ERR, "Invalid FSPM signature\n");
		return FSP_INCOMPATIBLE_VERSION;
	}

	/* Copy the default values from the UPD area */
	memcpy(&fspm_upd, upd, sizeof(fspm_upd));

	/* Give SoC and mainboard a chance to update the UPD */
	platform_fsp_memory_init_params_cb(&fspm_upd);

	/* Call FspMemoryInit */
	fsp_raminit = (void *)(hdr->image_base + hdr->memory_init_entry_offset);
	printk(BIOS_DEBUG, "Calling FspMemoryInit: 0x%p\n", fsp_raminit);
	printk(BIOS_SPEW, "\t%p: raminit_upd\n", &fspm_upd);
	printk(BIOS_SPEW, "\t%p: hob_list ptr\n", hob_list_ptr);

	timestamp_add_now(TS_FSP_MEMORY_INIT_START);
	status = fsp_raminit(&fspm_upd, hob_list_ptr);
	post_code(0x37);
	timestamp_add_now(TS_FSP_MEMORY_INIT_END);

	printk(BIOS_DEBUG, "FspMemoryInit returned 0x%08x\n", status);

	return status;
}

enum fsp_status fsp_memory_init(void **hob_list, struct range_entry *range)
{
	struct fsp_header hdr;

	/* TODO: do not hardcode CBFS file names */
	if (fsp_load_binary(&hdr, "blobs/fspm.bin", range) != CB_SUCCESS)
		return FSP_NOT_FOUND;

	return do_fsp_memory_init(hob_list, &hdr);
}
