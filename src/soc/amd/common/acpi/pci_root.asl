/* SPDX-License-Identifier: GPL-2.0-only */

#define ROOT_BRIDGE(acpi_name) \
	Device(acpi_name) { \
		Name(_HID, EISAID("PNP0A08"))	/* PCI Express Root Bridge */ \
		Name(_CID, EISAID("PNP0A03"))	/* PCI Root Bridge */ \
		Method (_OSC, 4, NotSerialized) { \
			/* Check for proper PCI/PCIe UUID */ \
			If (Arg0 == ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")) \
			{ \
				/* Let OS control everything */ \
				Return(Arg3) \
			} Else { \
				CreateDWordField(Arg3, 0, CDW1) \
				CDW1 = CDW1 | 4	/* Unrecognized UUID, so set bit 2 to 1 */ \
				Return(Arg3) \
			} \
		} \
	}
