/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <fsp/util.h>

void *cbmem_top_chipset(void)
{
	struct range_entry tolum;
	uint8_t *tolum_base;

	fsp_find_bootloader_tolum(&tolum);
	tolum_base = (uint8_t *)(uintptr_t)range_entry_base(&tolum);

	/*
	 * The TOLUM range may have other memory regions (such as APEI
	 * BERT region on top of CBMEM (IMD root and IMD small) region.
	 */
	return tolum_base + cbmem_overhead_size();
}
