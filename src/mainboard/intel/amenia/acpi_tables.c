/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include <soc/acpi.h>

void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	memset(fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = 5;
	memcpy(header->oem_id, OEM_ID, ARRAY_SIZE(header->oem_id));
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, ARRAY_SIZE(header->oem_table_id));
	memcpy(header->asl_compiler_id, ASLC, ARRAY_SIZE(header->asl_compiler_id));
	header->asl_compiler_revision = 1;

	fadt->firmware_ctrl = (uintptr_t)facs;
	fadt->dsdt = (uintptr_t)dsdt;
	fadt->model = 1;
	fadt->preferred_pm_profile = PM_MOBILE;

	fadt->x_firmware_ctl_l = (uintptr_t)facs;
	fadt->x_firmware_ctl_h = 0;
	fadt->x_dsdt_l = (uintptr_t)dsdt;
	fadt->x_dsdt_h = 0;

	soc_fill_common_fadt(fadt);

	header->checksum = acpi_checksum((void *)fadt, header->length);
}
