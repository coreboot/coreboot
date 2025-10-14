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

static void write_acpi_table(void)
{
	const size_t max_acpi_size = CONFIG_MAX_ACPI_TABLE_SIZE_KB * KiB;
	const uintptr_t acpi_start = (uintptr_t)cbmem_add(CBMEM_ID_ACPI, max_acpi_size);
	assert(IS_ALIGNED(acpi_start, 16));
	const uintptr_t acpi_end = write_acpi_tables(acpi_start);
	assert(acpi_end < acpi_start + max_acpi_size);
	printk(BIOS_DEBUG, "ACPI tables: %ld bytes.\n", acpi_end - acpi_start);
}

void arch_write_tables(uintptr_t coreboot_table)
{
	if (CONFIG(HAVE_ACPI_TABLES))
		write_acpi_table();
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
