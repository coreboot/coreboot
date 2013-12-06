/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Note: Only need HID on Primary Bus */
External (TOM1)
External (TOM2)
Name(_HID, EISAID("PNP0A03"))	/* PCI Express Root Bridge */
Name(_ADR, 0x00180000)	/* Dev# = BSP Dev#, Func# = 0 */

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
	If (PMOD)
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
		If(PMOD){ Return(APS2) }   /* APIC mode */
		Return (PS2)                  /* PIC Mode */
	} /* end _PRT */
} /* end PBR2 */

/* Dev4 GPP0 Root Port Bridge */
Device(PBR4) {
	Name(_ADR, 0x00040000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD){ Return(APS4) }	/* APIC mode */
		Return (PS4)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR4 */

/* Dev5 GPP1 Root Port Bridge */
Device(PBR5) {
	Name(_ADR, 0x00050000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD){ Return(APS5) }	/* APIC mode */
		Return (PS5)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR5 */

/* Dev6 GPP2 Root Port Bridge */
Device(PBR6) {
	Name(_ADR, 0x00060000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD){ Return(APS6) }	/* APIC mode */
		Return (PS6)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR6 */

/* The onboard EtherNet chip */
Device(PBR7) {
	Name(_ADR, 0x00070000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD){ Return(APS7) }	/* APIC mode */
		Return (PS7)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR7 */
