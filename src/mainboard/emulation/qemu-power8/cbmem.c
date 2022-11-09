/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>

uintptr_t cbmem_top_chipset(void)
{
	/* Top of cbmem is at lowest usable DRAM address below 4GiB. */
	/* For now, last 1M of 4G */
	return (1ULL << 32) - 1048576;
}
