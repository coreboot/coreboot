/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Device(EC)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 0)

	Name (_GPE, 0x17)
	Mutex (ECLK, 0)

	OperationRegion(ERAM, EmbeddedControl, 0x00, 0x100)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0x8),
		PAGE, 8,	/* Information Page Selector */
		Offset (0x70),
		    ,   1,
		LIDS,   1,
		    ,   3,
		HPAC,   1,
		Offset (0x88),
		B0PR,	1,	/* Battery 0 present */
		B0CH,	1,	/* Battery 0 charging */
		B0DI,	1,	/* Battery 0 discharging */
		Offset (0xA8),
		TMP0,	8,
		TMP1,	8,
	}

	Device(LID)
	{
		Name(_HID, "PNP0C0D")
		Method(_LID, 0, NotSerialized)
		{
			return (LIDS)
		}
	}

	Method(_Q52, 0, NotSerialized)
	{
		Notify(LID, 0x80)
	}

	Method(_Q53, 0, NotSerialized)
	{
		Notify(^LID, 0x80)
	}

	/* PAGE = 0 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xe0),
		BARC, 16,		/* Battery remaining capacity */
		BAFC, 16,		/* Battery full charge capacity */
		, 16,
		BAPR, 16,		/* Battery present rate */
		BAVO, 16,		/* Battery Voltage */
	}

	/* PAGE = 1 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xe0),
		BADC,	16,		/* Design Capacity */
		BADV,	16,		/* Design voltage */
		BASN,	16
	}

	/* PAGE = 2 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xe0),
		BANA,	128,		/* Battery name */
	}

	/* PAGE = 4 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xe0),
		BATY,	128,		/* Battery type */
	}

	/* PAGE = 5 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xe0),
		BAOE,	128,		/* Battery OEM info */
	}

	Method (_CRS, 0)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 1, 1)
			IO (Decode16, 0x66, 0x66, 1, 1)
		})
		Return (ECMD)
	}
	Method (_INI, 0, NotSerialized)
	{
	}

	/* Decrease brightness.  */
	Method(_Q1D, 0, NotSerialized)
	{
		\_SB.PCI0.GFX0.DECB()
	}
	/* Increase brightness.  */
	Method(_Q1C, 0, NotSerialized)
	{
		\_SB.PCI0.GFX0.INCB()
	}

#include "battery.asl"
#include "ac.asl"
#include "thermal.asl"
}
