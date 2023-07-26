/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/memmap.h>
#include <console/console.h>
#include <fsp/util.h>
#include <FspGuids.h>
#include <memrange.h>
#include <types.h>

void fsp_get_smm_region(uintptr_t *start, size_t *size)
{
	static uintptr_t smm_start;
	static size_t smm_size;

	*start = smm_start;
	*size = smm_size;
	if (*size && *start)
		return;

	struct range_entry tseg;

	if (fsp_find_range_hob(&tseg, AMD_FSP_TSEG_HOB_GUID.b) != CB_SUCCESS) {
		printk(BIOS_ERR, "unable to find TSEG HOB\n");
		return;
	}

	smm_start = (uintptr_t)range_entry_base(&tseg);
	smm_size =  range_entry_size(&tseg);
	*start = smm_start;
	*size = smm_size;
}
