/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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
 * Foundation, Inc.
 */

Device (LNKA)
{
	Name (_HID, EISAID("PNP0C0F"))
	Name (_UID, 1)

	// Disable method
	Method (_DIS, 0, Serialized)
	{
		Or (\_SB.PARC, 0x80, \_SB.PARC)
	}

	// Possible Resource Settings for this Link
	Name (_PRS, ResourceTemplate()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 12, 14, 15 }
	})

	// Current Resource Settings for this link
	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLA, 1, IRQ0)

		// Clear the WordField
		Store (Zero, IRQ0)

		// Set the bit from PRTA
		ShiftLeft (1, And (\_SB.PARC, 0x0f), IRQ0)

		Return (RTLA)
	}

	// Set Resource Setting for this IRQ link
	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)

		// Which bit is set?
		FindSetRightBit (IRQ0, Local0)

		Decrement(Local0)
		Store (Local0, \_SB.PARC)
	}

	// Status
	Method (_STA, 0, Serialized)
	{
		If(And (\_SB.PARC, 0x80)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKB)
{
	Name (_HID, EISAID("PNP0C0F"))
	Name (_UID, 2)

	// Disable method
	Method (_DIS, 0, Serialized)
	{
		Or (\_SB.PBRC, 0x80, \_SB.PBRC)
	}

	// Possible Resource Settings for this Link
	Name (_PRS, ResourceTemplate()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 12, 14, 15 }
	})

	// Current Resource Settings for this link
	Method (_CRS, 0, Serialized)
	{
		Name (RTLB, ResourceTemplate()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLB, 1, IRQ0)

		// Clear the WordField
		Store (Zero, IRQ0)

		// Set the bit from PRTB
		ShiftLeft (1, And (\_SB.PBRC, 0x0f), IRQ0)

		Return (RTLB)
	}

	// Set Resource Setting for this IRQ link
	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)

		// Which bit is set?
		FindSetRightBit (IRQ0, Local0)

		Decrement(Local0)
		Store (Local0, \_SB.PBRC)
	}

	// Status
	Method (_STA, 0, Serialized)
	{
		If(And (\_SB.PBRC, 0x80)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKC)
{
	Name (_HID, EISAID("PNP0C0F"))
	Name (_UID, 3)

	// Disable method
	Method (_DIS, 0, Serialized)
	{
		Or (\_SB.PCRC, 0x80, \_SB.PCRC)
	}

	// Possible Resource Settings for this Link
	Name (_PRS, ResourceTemplate()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 12, 14, 15 }
	})

	// Current Resource Settings for this link
	Method (_CRS, 0, Serialized)
	{
		Name (RTLC, ResourceTemplate()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLC, 1, IRQ0)

		// Clear the WordField
		Store (Zero, IRQ0)

		// Set the bit from PRTC
		ShiftLeft (1, And (\_SB.PCRC, 0x0f), IRQ0)

		Return (RTLC)
	}

	// Set Resource Setting for this IRQ link
	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)

		// Which bit is set?
		FindSetRightBit (IRQ0, Local0)

		Decrement(Local0)
		Store (Local0, \_SB.PCRC)
	}

	// Status
	Method (_STA, 0, Serialized)
	{
		If(And (\_SB.PCRC, 0x80)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKD)
{
	Name (_HID, EISAID("PNP0C0F"))
	Name (_UID, 4)

	// Disable method
	Method (_DIS, 0, Serialized)
	{
		Or (\_SB.PDRC, 0x80, \_SB.PDRC)
	}

	// Possible Resource Settings for this Link
	Name (_PRS, ResourceTemplate()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 12, 14, 15 }
	})

	// Current Resource Settings for this link
	Method (_CRS, 0, Serialized)
	{
		Name (RTLD, ResourceTemplate()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLD, 1, IRQ0)

		// Clear the WordField
		Store (Zero, IRQ0)

		// Set the bit from PRTD
		ShiftLeft (1, And (\_SB.PDRC, 0x0f), IRQ0)

		Return (RTLD)
	}

	// Set Resource Setting for this IRQ link
	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)

		// Which bit is set?
		FindSetRightBit (IRQ0, Local0)

		Decrement(Local0)
		Store (Local0, \_SB.PDRC)
	}

	// Status
	Method (_STA, 0, Serialized)
	{
		If(And (\_SB.PDRC, 0x80)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKE)
{
	Name (_HID, EISAID("PNP0C0F"))
	Name (_UID, 5)

	// Disable method
	Method (_DIS, 0, Serialized)
	{
		Or (\_SB.PERC, 0x80, \_SB.PERC)
	}

	// Possible Resource Settings for this Link
	Name (_PRS, ResourceTemplate()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 12, 14, 15 }
	})

	// Current Resource Settings for this link
	Method (_CRS, 0, Serialized)
	{
		Name (RTLE, ResourceTemplate()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLE, 1, IRQ0)

		// Clear the WordField
		Store (Zero, IRQ0)

		// Set the bit from PRTE
		ShiftLeft (1, And (\_SB.PERC, 0x0f), IRQ0)

		Return (RTLE)
	}

	// Set Resource Setting for this IRQ link
	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)

		// Which bit is set?
		FindSetRightBit (IRQ0, Local0)

		Decrement(Local0)
		Store (Local0, \_SB.PERC)
	}

	// Status
	Method (_STA, 0, Serialized)
	{
		If(And (\_SB.PERC, 0x80)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKF)
{
	Name (_HID, EISAID("PNP0C0F"))
	Name (_UID, 6)

	// Disable method
	Method (_DIS, 0, Serialized)
	{
		Or (\_SB.PFRC, 0x80, \_SB.PFRC)
	}

	// Possible Resource Settings for this Link
	Name (_PRS, ResourceTemplate()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 12, 14, 15 }
	})

	// Current Resource Settings for this link
	Method (_CRS, 0, Serialized)
	{
		Name (RTLF, ResourceTemplate()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLF, 1, IRQ0)

		// Clear the WordField
		Store (Zero, IRQ0)

		// Set the bit from PRTF
		ShiftLeft (1, And (\_SB.PFRC, 0x0f), IRQ0)

		Return (RTLF)
	}

	// Set Resource Setting for this IRQ link
	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)

		// Which bit is set?
		FindSetRightBit (IRQ0, Local0)

		Decrement(Local0)
		Store (Local0, \_SB.PFRC)
	}

	// Status
	Method (_STA, 0, Serialized)
	{
		If(And (\_SB.PFRC, 0x80)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKG)
{
	Name (_HID, EISAID("PNP0C0F"))
	Name (_UID, 7)

	// Disable method
	Method (_DIS, 0, Serialized)
	{
		Or (\_SB.PGRC, 0x80, \_SB.PGRC)
	}

	// Possible Resource Settings for this Link
	Name (_PRS, ResourceTemplate()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 12, 14, 15 }
	})

	// Current Resource Settings for this link
	Method (_CRS, 0, Serialized)
	{
		Name (RTLG, ResourceTemplate()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLG, 1, IRQ0)

		// Clear the WordField
		Store (Zero, IRQ0)

		// Set the bit from PRTG
		ShiftLeft (1, And (\_SB.PGRC, 0x0f), IRQ0)

		Return (RTLG)
	}

	// Set Resource Setting for this IRQ link
	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)

		// Which bit is set?
		FindSetRightBit (IRQ0, Local0)

		Decrement(Local0)
		Store (Local0, \_SB.PGRC)
	}

	// Status
	Method (_STA, 0, Serialized)
	{
		If(And (\_SB.PGRC, 0x80)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

Device (LNKH)
{
	Name (_HID, EISAID("PNP0C0F"))
	Name (_UID, 8)

	// Disable method
	Method (_DIS, 0, Serialized)
	{
		Or (\_SB.PHRC, 0x80, \_SB.PHRC)
	}

	// Possible Resource Settings for this Link
	Name (_PRS, ResourceTemplate()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 12, 14, 15 }
	})

	// Current Resource Settings for this link
	Method (_CRS, 0, Serialized)
	{
		Name (RTLH, ResourceTemplate()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLH, 1, IRQ0)

		// Clear the WordField
		Store (Zero, IRQ0)

		// Set the bit from PRTH
		ShiftLeft (1, And (\_SB.PHRC, 0x0f), IRQ0)

		Return (RTLH)
	}

	// Set Resource Setting for this IRQ link
	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)

		// Which bit is set?
		FindSetRightBit (IRQ0, Local0)

		Decrement(Local0)
		Store (Local0, \_SB.PHRC)
	}

	// Status
	Method (_STA, 0, Serialized)
	{
		If(And (\_SB.PHRC, 0x80)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}
}

