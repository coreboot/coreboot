/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <assert.h>
#include <boot/coreboot_tables.h>
#include <boot/tables.h>
#include <bootmem.h>
#include <cbmem.h>
#include <symbols.h>

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
	bootmem_add_range((uintptr_t)_ttb, REGION_SIZE(ttb), BM_MEM_RAMSTAGE);

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE) &&
	    REGION_SIZE(bl31) > 0)
		bootmem_add_range((uintptr_t)_bl31, REGION_SIZE(bl31),
				  BM_MEM_BL31);

	if (!CONFIG(COMMON_CBFS_SPI_WRAPPER))
		return;
	bootmem_add_range((uintptr_t)_postram_cbfs_cache,
			  REGION_SIZE(postram_cbfs_cache), BM_MEM_RAMSTAGE);
}

void lb_arch_add_records(struct lb_header *header)
{
}
