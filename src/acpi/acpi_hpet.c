/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/hpet.h>
#include <console/console.h>
#include <device/mmio.h>
#include <version.h>


/* http://www.intel.com/hardwaredesign/hpetspec_1.pdf */
static void acpi_create_hpet(acpi_hpet_t *hpet)
{
	acpi_header_t *header = &(hpet->header);
	acpi_addr_t *addr = &(hpet->addr);

	memset((void *)hpet, 0, sizeof(acpi_hpet_t));

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "HPET", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_hpet_t);
	header->revision = get_acpi_table_revision(HPET);

	/* Fill out HPET address. */
	addr->space_id = ACPI_ADDRESS_SPACE_MEMORY;
	addr->bit_width = 64;
	addr->bit_offset = 0;
	addr->addrl = HPET_BASE_ADDRESS & 0xffffffff;
	addr->addrh = ((unsigned long long)HPET_BASE_ADDRESS) >> 32;

	hpet->id = read32p(HPET_BASE_ADDRESS);
	hpet->number = 0;
	hpet->min_tick = CONFIG_HPET_MIN_TICKS;

	header->checksum = acpi_checksum((void *)hpet, sizeof(acpi_hpet_t));
}

unsigned long acpi_write_hpet(const struct device *device, unsigned long current,
			      acpi_rsdp_t *rsdp)
{
	acpi_hpet_t *hpet;

	/*
	 * We explicitly add these tables later on:
	 */
	printk(BIOS_DEBUG, "ACPI:    * HPET\n");

	hpet = (acpi_hpet_t *)current;
	current += sizeof(acpi_hpet_t);
	current = ALIGN_UP(current, 16);
	acpi_create_hpet(hpet);
	acpi_add_table(rsdp, hpet);

	return current;
}
