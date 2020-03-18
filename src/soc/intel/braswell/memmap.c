/*
 * This file is part of the coreboot project.
 *
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

static size_t smm_region_size(void)
{
	u32 smm_size;
	smm_size =  iosf_bunit_read(BUNIT_SMRRH) & 0xFFFF;
	smm_size -= iosf_bunit_read(BUNIT_SMRRL) & 0xFFFF;
	smm_size = (smm_size + 1) << 20;
	return smm_size;
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = (iosf_bunit_read(BUNIT_SMRRL) & 0xFFFF) << 20;
	*size = smm_region_size();
}

void *cbmem_top_chipset(void)
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
