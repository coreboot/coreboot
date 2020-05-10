/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/acpi.h>
#include <string.h>
#include <version.h>

void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	memset((void *) fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = get_acpi_table_revision(FADT);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = asl_revision;

	fadt->firmware_ctrl = (unsigned long)facs;
	fadt->dsdt = (unsigned long) dsdt;
	fadt->preferred_pm_profile = PM_MOBILE;

	fadt->x_firmware_ctl_l = (unsigned long)facs;
	fadt->x_firmware_ctl_h = 0;
	fadt->x_dsdt_l = (unsigned long)dsdt;
	fadt->x_dsdt_h = 0;

	acpi_fill_in_fadt(fadt);

	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES;

	header->checksum = acpi_checksum((void *)fadt, header->length);
}
