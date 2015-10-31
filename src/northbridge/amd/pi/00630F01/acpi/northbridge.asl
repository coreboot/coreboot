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

/* Dev3 GPP Root Port Bridge */
Device(PBR3) {
	Name(_ADR, 0x00030000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD){ Return(APS3) }	/* APIC mode */
		Return (PS3)				/* PIC Mode */
	} /* end _PRT */
} /* end PBR3 */
