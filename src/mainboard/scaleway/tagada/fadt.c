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
 *
 */

#include <arch/acpi.h>

#include <soc/acpi.h>
#include <soc/soc_util.h>

void motherboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->reserved = 0;
	fadt->preferred_pm_profile = PM_ENTERPRISE_SERVER;
}
