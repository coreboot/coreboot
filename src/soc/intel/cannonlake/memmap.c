/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2017 Intel Corporation.
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
#include <arch/io.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/bootblock.h>
#include <soc/systemagent.h>

static void *top_of_ram_register(void)
{
	int num;
	int offset;
	num = (read32((uintptr_t *)HPET_BASE_ADDRESS) >> 8) & 0x1f;
	offset = 0x100 + (0x20 * num) + 0x08;
	return (void *)(uintptr_t)(HPET_BASE_ADDRESS + offset);
}

void clear_cbmem_top(void)
{
	write32(top_of_ram_register(), 0);
}

void cbmem_top_init(void)
{
	struct range_entry fsp_mem;
	uintptr_t top;

	if (fsp_find_reserved_memory(&fsp_mem))
		die("Can't file top of ram.\n");

	top = ALIGN_UP(range_entry_base(&fsp_mem), 16 * MiB);
	write32(top_of_ram_register(), top);
}

void *cbmem_top(void)
{
	/*
	 *     +-------------------------+  Top of RAM (aligned)
	 *     | System Management Mode  |
	 *     |      code and data      |  Length: CONFIG_TSEG_SIZE
	 *     |         (TSEG)          |
	 *     +-------------------------+  SMM base (aligned)
	 *     |                         |
	 *     | Chipset Reserved Memory |
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
	return (void *)(uintptr_t)read32(top_of_ram_register());
}
