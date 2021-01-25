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

/* Dev4 GPP0 Root Port Bridge */
Device(PBR4) {
	Name(_ADR, 0x00040000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APS4) }	/* APIC mode */
		Return (PS4)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR4 */

/* Dev5 GPP1 Root Port Bridge */
Device(PBR5) {
	Name(_ADR, 0x00050000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APS5) }	/* APIC mode */
		Return (PS5)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR5 */

/* Dev6 GPP2 Root Port Bridge */
Device(PBR6) {
	Name(_ADR, 0x00060000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APS6) }	/* APIC mode */
		Return (PS6)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR6 */

/* The onboard EtherNet chip */
Device(PBR7) {
	Name(_ADR, 0x00070000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APS7) }	/* APIC mode */
		Return (PS7)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR7 */

Device(K10M) {
	Name (_ADR, 0x00180003)
	#include <soc/amd/common/acpi/thermal_zone.asl>
}
