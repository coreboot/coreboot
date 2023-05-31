/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

Device(PCI0) {
	Name(_HID, EISAID("PNP0A08"))	/* PCI Express Root Bridge */
	Name(_CID, EISAID("PNP0A03"))	/* PCI Root Bridge */

	/* Operating System Capabilities Method */
	Method(_OSC, 4) {
		CreateDWordField(Arg3, 0, CDW1) /* Capabilities dword 1 */

		/* Check for proper PCI/PCIe UUID */
		If (Arg0 == ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")) {
			/* Let OS control everything */
			Return (Arg3)
		} Else {
			CDW1 |= 4	/* Unrecognized UUID */
			Return (Arg3)
		}
	}

	/* 0:14.3 - LPC */
	#include <soc/amd/common/acpi/lpc.asl>

} /* End PCI0 scope */
