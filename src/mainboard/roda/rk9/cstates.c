/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/acpigen.h>
#include <device/device.h> /* fix for i82801ix.h */
#include <southbridge/intel/i82801ix/i82801ix.h>

static acpi_cstate_t cst_entries[] = {
	{
		/* acpi C1 / cpu C1 */
		1, 0x01, 1000,
		{ ACPI_ADDRESS_SPACE_FIXED, 1, 2, { 1 }, 0, 0 }
	},
	{
		/* acpi C2 / cpu C2 */
		2, 0x01,  500,
		{ ACPI_ADDRESS_SPACE_FIXED, 1, 2, { 1 }, 0x10, 0 }
	},
};

int get_cst_entries(acpi_cstate_t **entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}
