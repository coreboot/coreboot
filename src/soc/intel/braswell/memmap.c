/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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
#include <cpu/x86/smm.h>
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

void smm_region(uintptr_t *start, size_t *size)
{
	*start = (iosf_bunit_read(BUNIT_SMRRL) & 0xFFFF) << 20;
	*size = smm_region_size();
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
int smm_subregion(int sub, uintptr_t *start, size_t *size)
{
	uintptr_t sub_base;
	size_t sub_size;
	const size_t cache_size = CONFIG_SMM_RESERVED_SIZE;

	smm_region(&sub_base, &sub_size);

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
		*start = 0;
		*size = 0;
		return -1;
	}

	*start = sub_base;
	*size = sub_size;
	return 0;
}

void *cbmem_top(void)
{
	uintptr_t smm_base;
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

	smm_region(&smm_base, &smm_size);
	return (void *)smm_base;
}
