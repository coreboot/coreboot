/*
 * This file is part of the coreboot project.
 *
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

#include <string.h>
#include <soc/acpi.h>
#include <version.h>

void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	/* Initialize the FADT header */
	memset((void *) fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = ACPI_FADT_REV_ACPI_5_0;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = asl_revision;

	/* Fill in SoC specific values */
	acpi_fill_in_fadt(fadt);

	header->checksum = acpi_checksum((void *) fadt, header->length);
}
