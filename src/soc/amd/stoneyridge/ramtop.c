/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
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

#define __SIMPLE_DEVICE__

#include <assert.h>
#include <stdint.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cbmem.h>
#include <soc/northbridge.h>

#define CBMEM_TOP_SCRATCHPAD 0x78

void backup_top_of_low_cacheable(uintptr_t ramtop)
{
	uint16_t top_cache = ramtop >> 16;
	pci_write_config16(PCI_DEV(0,0,0), CBMEM_TOP_SCRATCHPAD, top_cache);
}

uintptr_t restore_top_of_low_cacheable(void)
{
	uint16_t top_cache;
	top_cache = pci_read_config16(PCI_DEV(0,0,0), CBMEM_TOP_SCRATCHPAD);
	return (top_cache << 16);
}

void *cbmem_top(void)
{
	msr_t tom = rdmsr(TOP_MEM);

	if (!tom.lo)
		return 0;
	else
		/* 8MB alignment to keep MTRR usage low */
		return (void *)ALIGN_DOWN(restore_top_of_low_cacheable()
				- CONFIG_SMM_TSEG_SIZE, 8*MiB);
}

static uintptr_t smm_region_start(void)
{
	return (uintptr_t)cbmem_top();
}

static size_t smm_region_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
}

void smm_region_info(void **start, size_t *size)
{
	*start = (void *)smm_region_start();
	*size = smm_region_size();
}

int smm_subregion(int sub, void **start, size_t *size)
{
	uintptr_t sub_base;
	size_t sub_size;
	const size_t cache_size = CONFIG_SMM_RESERVED_SIZE;

	sub_base = smm_region_start();
	sub_size = smm_region_size();

	assert(sub_size > CONFIG_SMM_RESERVED_SIZE);

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
