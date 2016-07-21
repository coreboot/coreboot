/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corporation.
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
#include <fsp/memmap.h>
#include <soc/QuarkNcSocId.h>
#include <soc/reg_access.h>

size_t mmap_region_granularity(void)
{
	/* Align to 8 MiB by default */
	return 8 << 20;
}

void *cbmem_top(void)
{
	uint32_t top_of_memory;

	/* Determine the TSEG base */
	top_of_memory = reg_host_bridge_unit_read(QNC_MSG_FSBIC_REG_HSMMC);
	top_of_memory &= SMM_START_MASK;
	top_of_memory <<= 16;

	/* Reserve 64 KiB for RMU firmware */
	top_of_memory -= 0x10000;

	/* Return the top of memory */
	return (void *)top_of_memory;
}
