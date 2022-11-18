/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <console/console.h>
#include <acpi/acpi.h>
#include <device/device.h>

#include "mainboard.h"

static long acpi_create_ecdt(acpi_ecdt_t * ecdt)
{
	/* Attention: Make sure these match the values from
	 * the DSDT's ec.asl
	 */
	static const char ec_id[] = "\\_SB.PCI0.LPCB.EC0";
	int ecdt_len = sizeof(acpi_ecdt_t) + strlen(ec_id) + 1;

	acpi_header_t *header = &(ecdt->header);

	memset((void *)ecdt, 0, ecdt_len);

	/* fill out header fields */
	memcpy(header->signature, "ECDT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = ecdt_len;
	header->revision = 1;

	/* Location of the two EC registers */
	ecdt->ec_control.space_id = ACPI_ADDRESS_SPACE_IO;
	ecdt->ec_control.bit_width = 8;
	ecdt->ec_control.bit_offset = 0;
	ecdt->ec_control.addrl = 0x66;
	ecdt->ec_control.addrh = 0;

	ecdt->ec_data.space_id = ACPI_ADDRESS_SPACE_IO;
	ecdt->ec_data.bit_width = 8;
	ecdt->ec_data.bit_offset = 0;
	ecdt->ec_data.addrl = 0x62;
	ecdt->ec_data.addrh = 0;

	ecdt->uid = 1; // Must match _UID of the EC0 node.

	ecdt->gpe_bit = 23; // SCI interrupt within GPEx_STS

	memcpy(ecdt->ec_id, ec_id, sizeof(ec_id));

	header->checksum =
	    acpi_checksum((void *)ecdt, ecdt_len);

	return header->length;
}

unsigned long mainboard_write_acpi_tables(const struct device *device,
					  unsigned long start,
					  acpi_rsdp_t *rsdp)
{
	unsigned long current;
	acpi_header_t *ecdt;

	current = start;

	/* Align ACPI tables to 16byte */
	current = acpi_align_current(current);

	printk(BIOS_DEBUG, "ACPI:     * ECDT\n");
	ecdt = (acpi_header_t *)current;
	current += acpi_create_ecdt((acpi_ecdt_t *)current);
	current = acpi_align_current(current);
	acpi_add_table(rsdp, ecdt);

	printk(BIOS_DEBUG, "current = %lx\n", current);
	return current;
}
