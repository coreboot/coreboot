/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>

int fsp_find_bootloader_tolum(struct range_entry *re)
{
	return fsp_find_range_hob(re, fsp_bootloader_tolum_guid);
}

void fsp_verify_memory_init_hobs(void)
{
	struct range_entry fsp_mem;
	struct range_entry tolum;

	/* Lookup the FSP_BOOTLOADER_TOLUM_HOB */
	if (fsp_find_bootloader_tolum(&tolum))
		die("9.3: FSP_BOOTLOADER_TOLUM_HOB missing!\n");
	if (range_entry_size(&tolum) < cbmem_overhead_size()) {
		printk(BIOS_CRIT,
			"FSP_BOOTLOADER_TOLUM_SIZE: 0x%08llx < 0x%08zx\n",
			range_entry_size(&tolum), cbmem_overhead_size());
		die("FSP_BOOTLOADER_TOLUM_HOB too small!\n");
	}

	/* Locate the FSP reserved memory area */
	if (fsp_find_reserved_memory(&fsp_mem))
		die("9.1: FSP_RESERVED_MEMORY_RESOURCE_HOB missing!\n");

	/* Verify the bootloader tolum is above the FSP reserved area */
	if (range_entry_end(&tolum) <= range_entry_base(&fsp_mem)) {
		printk(BIOS_CRIT,
			"TOLUM end: 0x%08llx != 0x%08llx: FSP rsvd base\n",
			range_entry_end(&tolum), range_entry_base(&fsp_mem));
		die("FSP reserved region after BIOS TOLUM!\n");
	}
	if (range_entry_base(&tolum) < range_entry_end(&fsp_mem)) {
		printk(BIOS_CRIT,
			"TOLUM base: 0x%08llx < 0x%08llx: FSP rsvd end\n",
			range_entry_base(&tolum), range_entry_end(&fsp_mem));
		die("FSP reserved region overlaps BIOS TOLUM!\n");
	}

	/* Verify that the FSP reserved area immediately follows the BIOS
	 * reserved area
	 */
	if (range_entry_base(&tolum) != range_entry_end(&fsp_mem)) {
		printk(BIOS_CRIT,
			"TOLUM base: 0x%08llx != 0x%08llx: FSP rsvd end\n",
			range_entry_base(&tolum), range_entry_end(&fsp_mem));
		die("Space between FSP reserved region and BIOS TOLUM!\n");
	}

	if (range_entry_end(&tolum) != (uintptr_t)cbmem_top()) {
		printk(BIOS_CRIT, "TOLUM end: 0x%08llx != 0x%p: cbmem_top\n",
			range_entry_end(&tolum), cbmem_top());
		die("Space between cbmem_top and BIOS TOLUM!\n");
	}
}
