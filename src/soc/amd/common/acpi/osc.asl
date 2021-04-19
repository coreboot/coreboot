/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_osc.h>

/*
 * Platform-Wide _OSC Operating System Capabilities
 *
 * Arg0: A Buffer containing a UUID
 * Arg1: An Integer containing a Revision ID of the buffer format
 * Arg2: An Integer containing a count of entries in Arg3
 * Arg3: A Buffer containing a list of DWORD capabilities
 */
Method (_OSC, 4, NotSerialized) {
	CreateDWordField (Arg3, 0, CDW1)
	If (Arg0 == ToUUID (OSC_SB_UUID)) {
		/* Don't mask any capability bits off. */
		Return (Arg3)
	} Else {
		CDW1 |= OSC_CDW1_UNRECOGNIZED_UUID
		Return (Arg3)
	}
}
