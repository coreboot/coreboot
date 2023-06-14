/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <assert.h>
#include <boot/coreboot_tables.h>
#include <boot/tables.h>
#include <bootmem.h>
#include <cbmem.h>
#include <console/console.h>
#include <smbios.h>
#include <string.h>
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

static void write_smbios_table(void)
{
	unsigned long smbios_begin, smbios_end;

#define MAX_SMBIOS_SIZE (32 * KiB)

	smbios_begin = (unsigned long)cbmem_add(CBMEM_ID_SMBIOS, MAX_SMBIOS_SIZE);
	if (!smbios_begin) {
		printk(BIOS_ERR, "Out of memory for SMBIOS tables\n");
		return;
	}

	/*
	 * Clear the entire region to ensure the unused space doesn't
	 * contain garbage from a previous boot, like stale table
	 * signatures that could be found by the OS.
	 */
	memset((void *)smbios_begin, 0, MAX_SMBIOS_SIZE);

	smbios_end = smbios_write_tables(smbios_begin);

	if (smbios_end > (smbios_begin + MAX_SMBIOS_SIZE))
		printk(BIOS_ERR, "Increase SMBIOS size\n");

	printk(BIOS_DEBUG, "SMBIOS tables: %ld bytes.\n", smbios_end - smbios_begin);
}

void arch_write_tables(uintptr_t coreboot_table)
{
	if (CONFIG(HAVE_ACPI_TABLES))
		write_acpi_table();

	if (CONFIG(GENERATE_SMBIOS_TABLES))
		write_smbios_table();
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
