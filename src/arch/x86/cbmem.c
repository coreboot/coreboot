/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>

#if CONFIG(CBMEM_TOP_BACKUP)

void *cbmem_top_chipset(void)
{
	/* Top of CBMEM is at highest usable DRAM address below 4GiB. */
	return (void *)restore_top_of_low_cacheable();
}

#endif /* CBMEM_TOP_BACKUP */
