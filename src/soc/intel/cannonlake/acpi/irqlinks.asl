/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
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

/*
 * PIRQ routing control is in PCR ITSS region.
 *
 * Due to what appears to be an ACPI interpreter bug we do not use
 * the PCRB() method here as it may not be defined yet because the method
 * definiton depends on the order of the include files in pch.asl.
 *
 * https://bugs.acpica.org/show_bug.cgi?id=1201
 */
OperationRegion (ITSS, SystemMemory,
		 Add (PCR_ITSS_PIRQA_ROUT,
		      Add (CONFIG_PCR_BASE_ADDRESS,
		           ShiftLeft (PID_ITSS, PCR_PORTID_SHIFT))), 8)
Field (ITSS, ByteAcc, NoLock, Preserve)
{
	PIRA, 8,	/* PIRQA Routing Control */
	PIRB, 8,	/* PIRQB Routing Control */
	PIRC, 8,	/* PIRQC Routing Control */
	PIRD, 8,	/* PIRQD Routing Control */
	PIRE, 8,	/* PIRQE Routing Control */
	PIRF, 8,	/* PIRQF Routing Control */
	PIRG, 8,	/* PIRQG Routing Control */
	PIRH, 8,	/* PIRQH Routing Control */
}

Name (IREN, 0x80)	/* Interrupt Routing Enable */
Name (IREM, 0x0f)	/* Interrupt Routing Mask */

Device (LNKA)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 1)

	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {11}
		})
		CreateWordField (RTLA, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRA, ^^IREM), IRQ0)

		Return (RTLA)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRA, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKB)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 2)

	Method (_CRS, 0, Serialized)
	{
		Name (RTLB, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {10}
		})
		CreateWordField (RTLB, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRB, ^^IREM), IRQ0)

		Return (RTLB)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRB, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKC)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 3)

	Method (_CRS, 0, Serialized)
	{
		Name (RTLC, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {11}
		})
		CreateWordField (RTLC, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRC, ^^IREM), IRQ0)

		Return (RTLC)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRC, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKD)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 4)

	Method (_CRS, 0, Serialized)
	{
		Name (RTLD, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {11}
		})
		CreateWordField (RTLD, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRD, ^^IREM), IRQ0)

		Return (RTLD)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRD, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKE)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 5)

	Method (_CRS, 0, Serialized)
	{
		Name (RTLE, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {11}
		})
		CreateWordField (RTLE, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRE, ^^IREM), IRQ0)

		Return (RTLE)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRE, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKF)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 6)

	Method (_CRS, 0, Serialized)
	{
		Name (RTLF, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {11}
		})
		CreateWordField (RTLF, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRF, ^^IREM), IRQ0)

		Return (RTLF)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRF, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKG)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 7)

	Method (_CRS, 0, Serialized)
	{
		Name (RTLG, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {11}
		})
		CreateWordField (RTLG, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRG, ^^IREM), IRQ0)

		Return (RTLG)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRG, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKH)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 8)

	Method (_CRS, 0, Serialized)
	{
		Name (RTLH, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {11}
		})
		CreateWordField (RTLH, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRH, ^^IREM), IRQ0)

		Return (RTLH)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRH, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}
