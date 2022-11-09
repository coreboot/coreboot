/* SPDX-License-Identifier: GPL-2.0-only */

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

uintptr_t cbmem_top_chipset(void)
{
	uintptr_t smm_base;
	size_t smm_size;

	/*
	 *     +-------------------------+  Top of RAM (aligned)
	 *     | System Management Mode  |
	 *     |      code and data      |  Length: CONFIG_SMM_TSEG_SIZE
	 *     |         (TSEG)          |
	 *     +-------------------------+  SMM base (aligned)
	 *     |                         |
	 *     | Chipset Reserved Memory |  Length: Multiple of CONFIG_SMM_TSEG_SIZE
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
	return smm_base;
}
