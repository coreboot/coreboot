/* SPDX-License-Identifier: GPL-2.0-only */

/* Note: Only need HID on Primary Bus */
External (TOM1)
External (TOM2)
Name(_HID, EISAID("PNP0A03"))	/* PCI Express Root Bridge */

/* Describe the Northbridge devices */

Method (_BBN, 0, NotSerialized)
{
	Return (Zero)
}

Method (_STA, 0, NotSerialized)
{
	Return (0x0B)
}

Method (_PRT, 0, NotSerialized)
{
	If (PICM)
	{
		Return (APR0)
	}

	Return (PR0)
}

Device(AMRT) {
	Name(_ADR, 0x00000000)
} /* end AMRT */

/* Dev2 is also an external GFX bridge */
Device(PBR2) {
	Name(_ADR, 0x00020000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APS2) }   /* APIC mode */
		Return (PS2)                  /* PIC Mode */
	} /* end _PRT */
} /* end PBR2 */

/* Dev3 GPP Root Port Bridge */
Device(PBR3) {
	Name(_ADR, 0x00030000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APS3) }	/* APIC mode */
		Return (PS3)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR3 */

Device(K10M) {
	Name (_ADR, 0x00180003)
	#include <soc/amd/common/acpi/thermal_zone.asl>
}
