/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <fsp/util.h>

void *cbmem_top_chipset(void)
{
	struct range_entry tolum;

	fsp_find_bootloader_tolum(&tolum);
	return (void *)(uintptr_t)range_entry_end(&tolum);
}
