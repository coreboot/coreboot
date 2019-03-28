/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 * Copyright (C) 2005 Steve Magnani
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <bootmem.h>
#include <boot/tables.h>
#include <boot/coreboot_tables.h>
#include <symbols.h>

DECLARE_OPTIONAL_REGION(opensbi);

void arch_write_tables(uintptr_t coreboot_table)
{
}

void bootmem_arch_add_ranges(void)
{
	if (CONFIG(RISCV_OPENSBI) && REGION_SIZE(opensbi) > 0)
		bootmem_add_range((uintptr_t)_opensbi, REGION_SIZE(opensbi),
				  BM_MEM_OPENSBI);
}

void lb_arch_add_records(struct lb_header *header)
{
}
