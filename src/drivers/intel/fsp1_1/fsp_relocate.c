/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/fsp.h>
#include <fsp/util.h>

int fsp_relocate(struct prog *fsp_relocd)
{
	void *fih;
	ssize_t fih_offset;
	size_t size;

	void *new_loc = cbfs_cbmem_alloc(prog_name(fsp_relocd),
					 CBMEM_ID_REFCODE, &size);
	if (new_loc == NULL) {
		printk(BIOS_ERR, "ERROR: Unable to load FSP into memory.\n");
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
