/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <stdint.h>
#include <console/console.h>
#include <arch/bert_storage.h>
#include <memrange.h>
#include <fsp/util.h>
#include <FspGuids.h>

void bert_reserved_region(void **start, size_t *size)
{
	struct range_entry bert;
	int status;

	*start = NULL;
	*size = 0;

	status = fsp_find_range_hob(&bert, AMD_FSP_BERT_HOB_GUID.b);

	if (status < 0) {
		printk(BIOS_ERR, "Error: unable to find BERT HOB\n");
		return;
	}

	*start = (void *)(uintptr_t)range_entry_base(&bert);
	*size = range_entry_size(&bert);
}
