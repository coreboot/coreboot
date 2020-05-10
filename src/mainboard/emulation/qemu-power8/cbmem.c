/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>

void *cbmem_top_chipset(void)
{
	/* Top of cbmem is at lowest usable DRAM address below 4GiB. */
	/* For now, last 1M of 4G */
	void *ptr = (void *) ((1ULL << 32) - 1048576);
	return ptr;
}
