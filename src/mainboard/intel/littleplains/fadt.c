/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#include <device/device.h>
#include <southbridge/intel/fsp_rangeley/soc.h>

void acpi_create_fadt(acpi_fadt_t * fadt, acpi_facs_t * facs, void *dsdt)
{
	acpi_fill_in_fadt(fadt,facs,dsdt);

#define PLATFORM_HAS_FADT_CUSTOMIZATIONS	0


	/*
	 * Platform specific customizations go here.
	 * Update the #define above if customizations are added.
	 */


#if PLATFORM_HAS_FADT_CUSTOMIZATIONS
	header->checksum = 0;
	header->checksum =
		acpi_checksum((void *) fadt, sizeof(acpi_fadt_t));
#endif

}
