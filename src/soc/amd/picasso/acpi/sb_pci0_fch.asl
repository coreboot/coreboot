/* SPDX-License-Identifier: GPL-2.0-only */

/* System Bus */
/*  _SB.PCI0 */

/* Operating System Capabilities Method */
Method(_OSC,4)
{
	/* Check for proper PCI/PCIe UUID */
	If (Arg0 == ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
	{
		/* Let OS control everything */
		Return (Arg3)
	} Else {
		CreateDWordField(Arg3,0,CDW1)
		CDW1 |= 4	// Unrecognized UUID
		Return (Arg3)
	}
}

/* 0:14.3 - LPC */
#include <soc/amd/common/acpi/lpc.asl>
#include <soc/amd/common/acpi/platform.asl>
