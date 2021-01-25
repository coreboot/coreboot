/* SPDX-License-Identifier: GPL-2.0-only */

/* Note: Only need HID on Primary Bus */
External (TOM1)
External (TOM2)
Name(_HID, EISAID("PNP0A08"))	/* PCI Express Root Bridge */
Name(_CID, EISAID("PNP0A03"))	/* PCI Root Bridge */

/* Describe the Northbridge devices */
Device(AMRT) {
	Name(_ADR, 0x00000000)
} /* end AMRT */

/* The internal GFX bridge */
Device(AGPB) {
	Name(_ADR, 0x00010000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APR1) }	/* APIC mode */
		Return (PR1)				/* PIC Mode */
	}
}  /* end AGPB */

/* The internal GFX bridge */
Device(HDMI) {
	Name(_ADR, 0x00010001)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APR1) }	/* APIC mode */
		Return (PR1)				/* PIC Mode */
	}
}  /* end HDMI */

/* Dev 2 & 3 are external GFX bridges, not used in Family14 */

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

Device(PE20) {
	Name(_ADR, 0x00150000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APE0) }	/* APIC mode */
		Return (PE0)				/* PIC Mode */
	} /* end _PRT */
} /* end PE20 */

Device(PE21) {
	Name(_ADR, 0x00150001)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APE1) }	/* APIC mode */
		Return (PE1)				/* PIC Mode */
	} /* end _PRT */
} /* end PE21 */

Device(PE22) {
	Name(_ADR, 0x00150002)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APE2) }	/* APIC mode */
		Return (APE2)				/* PIC Mode */
	} /* end _PRT */
} /* end PE22 */

Device(PE23) {
	Name(_ADR, 0x00150003)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PICM) { Return(APE3) }	/* APIC mode */
		Return (PE3)				/* PIC Mode */
	} /* end _PRT */
} /* end PE23 */

/* Northbridge function 3 */
Device(NBF3) {
	Name(_ADR, 0x00180003)
	/* k10temp thermal zone */
	#include <soc/amd/common/acpi/thermal_zone.asl>
} /* end NBF3 */
