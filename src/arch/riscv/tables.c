/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmem.h>
#include <boot/tables.h>
#include <boot/coreboot_tables.h>
#include <symbols.h>
#include <assert.h>
#include <acpi/acpi.h>
#include <cbmem.h>
#include <console/console.h>

unsigned long acpi_arch_fill_madt(acpi_madt_t *madt, unsigned long current)
{
	return current;
}

void arch_write_tables(uintptr_t coreboot_table)
{
	if (CONFIG(HAVE_ACPI_TABLES))
		acpi_allocate_write_tables();
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
