/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
