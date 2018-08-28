/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <cbmem.h>
#include <commonlib/fsp.h>
#include <fsp/util.h>

int fsp_relocate(struct prog *fsp_relocd, const struct region_device *fsp_src)
{
	void *new_loc;
	void *fih;
	ssize_t fih_offset;
	size_t size = region_device_sz(fsp_src);

	new_loc = cbmem_add(CBMEM_ID_REFCODE, size);

	if (new_loc == NULL) {
		printk(BIOS_ERR, "ERROR: Unable to load FSP into memory.\n");
		return -1;
	}

	if (rdev_readat(fsp_src, new_loc, 0, size) != size) {
		printk(BIOS_ERR, "ERROR: Can't read FSP's region device.\n");
		return -1;
	}

	fih_offset = fsp1_1_relocate((uintptr_t)new_loc, new_loc, size);

	if (fih_offset <= 0) {
		printk(BIOS_ERR, "ERROR: FSP relocation failure.\n");
		return -1;
	}

	fih = (void *)((uint8_t *)new_loc + fih_offset);

	prog_set_area(fsp_relocd, new_loc, size);
	prog_set_entry(fsp_relocd, fih, NULL);

	return 0;
}
