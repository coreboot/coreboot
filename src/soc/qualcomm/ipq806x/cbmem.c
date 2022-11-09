/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <soc/soc_services.h>

static int cbmem_backing_store_ready;

void ipq_cbmem_backing_store_ready(void)
{
	cbmem_backing_store_ready = 1;
}

uintptr_t cbmem_top_chipset(void)
{
	/*
	 * In romstage, make sure that cbmem backing store is ready before
	 * returning the pointer to cbmem top. Otherwise, it could lead to
	 * issues with components that utilize cbmem in romstage
	 * (e.g. vboot_locator for loading ipq blobs before DRAM is
	 * initialized).
	 */
	if (cbmem_backing_store_ready == 0)
		return 0;

	return (uintptr_t)_memlayout_cbmem_top;
}
