/* SPDX-License-Identifier: GPL-2.0-only */

/* Elan trackpad */
#include <acpi/trackpad_elan.asl>

/* Realtek audio codec */
#include <acpi/codec_realtek.asl>

Scope (\_SB)
{
	Method (GPID, 0, Serialized)  // GPID: Get Project ID for Terra2/Terra3
	{
		And(\BDID >> 3, 0x01, Local0)
		Return (Local0)
	}
}
