/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Elan trackpad */
#include <acpi/trackpad_elan.asl>

/* Realtek audio codec */
#include <acpi/codec_realtek.asl>

Scope (\_SB)
{
	Method (GPID, 0, Serialized)  // GPID: Get Project ID for Terra2/Terra3
	{
		And( ShiftRight (\BDID, 3, Local0), 0x01, Local0)
		Return (Local0)
	}
}
