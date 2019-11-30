/*
 * This file is part of the coreboot project.
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

#include <cbmem.h>

#if CONFIG(CBMEM_TOP_BACKUP)

void *cbmem_top_chipset(void)
{
	/* Top of CBMEM is at highest usable DRAM address below 4GiB. */
	return (void *)restore_top_of_low_cacheable();
}

#endif /* CBMEM_TOP_BACKUP */
