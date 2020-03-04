/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
