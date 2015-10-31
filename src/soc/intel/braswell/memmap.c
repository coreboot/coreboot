/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied wacbmem_entryanty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/memmap.h>
#include <soc/iosf.h>
#include <soc/smm.h>

static size_t smm_region_size(void)
{
	u32 smm_size;
	smm_size = iosf_bunit_read(BUNIT_SMRRH) & 0xFFFF;
	smm_size -= iosf_bunit_read(BUNIT_SMRRL) & 0xFFFF;
	smm_size = (smm_size + 1) << 20;
	return smm_size;
}

void smm_region(void **start, size_t *size)
{
	*start = (void *)((iosf_bunit_read(BUNIT_SMRRL) & 0xFFFF) << 20);
	*size = smm_region_size();
}

size_t mmap_region_granluarity(void)
{
	/* Align to TSEG size when SMM is in use, and 8MiB by default */
	return IS_ENABLED(CONFIG_HAVE_SMI_HANDLER) ? smm_region_size()
		: 8 << 20;
}

/*
 *        Subregions within SMM
 *     +-------------------------+ BUNIT_SMRRH
 *     |  External Stage Cache   | SMM_RESERVED_SIZE
 *     +-------------------------+
 *     |      code and data      |
 *     |         (TSEG)          |
 *     +-------------------------+ BUNIT_SMRRL
 */
int smm_subregion(int sub, void **start, size_t *size)
{
	uintptr_t sub_base;
	void *sub_ptr;
	size_t sub_size;
	const size_t cache_size = CONFIG_SMM_RESERVED_SIZE;

	smm_region(&sub_ptr, &sub_size);
	sub_base = (uintptr_t)sub_ptr;

	switch (sub) {
	case SMM_SUBREGION_HANDLER:
		/* Handler starts at the base of TSEG. */
		sub_size -= cache_size;
		break;
	case SMM_SUBREGION_CACHE:
		/* External cache is in the middle of TSEG. */
		sub_base += sub_size - cache_size;
		sub_size = cache_size;
		break;
	default:
		return -1;
	}

	*start = (void *)sub_base;
	*size = sub_size;

	return 0;
}

void *cbmem_top(void)
{
	char *smm_base;
	size_t smm_size;

	/*
	 *     +-------------------------+  Top of RAM (aligned)
	 *     | System Management Mode  |
	 *     |      code and data      |  Length: CONFIG_TSEG_SIZE
	 *     |         (TSEG)          |
	 *     +-------------------------+  SMM base (aligned)
	 *     |                         |
	 *     | Chipset Reserved Memory |  Length: Multiple of CONFIG_TSEG_SIZE
	 *     |                         |
	 *     +-------------------------+  top_of_ram (aligned)
	 *     |                         |
	 *     |       CBMEM Root        |
	 *     |                         |
	 *     +-------------------------+
	 *     |                         |
	 *     |   FSP Reserved Memory   |
	 *     |                         |
	 *     +-------------------------+
	 *     |                         |
	 *     |  Various CBMEM Entries  |
	 *     |                         |
	 *     +-------------------------+  top_of_stack (8 byte aligned)
	 *     |                         |
	 *     |   stack (CBMEM Entry)   |
	 *     |                         |
	 *     +-------------------------+
	*/

	smm_region((void **)&smm_base, &smm_size);
	return (void *)smm_base;
}
