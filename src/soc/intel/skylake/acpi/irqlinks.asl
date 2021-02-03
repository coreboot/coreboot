/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * PIRQ routing control is in PCR ITSS region.
 *
 * Due to what appears to be an ACPI interpreter bug we do not use
 * the PCRB() method here as it may not be defined yet because the method
 * definition depends on the order of the include files in pch.asl.
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

	Name (_PRS, ResourceTemplate ()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 11, 12, 14, 15 }
	})

	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLA, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRA, ^^IREM), IRQ0)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Decrement (Local0)
		Store (Local0, ^^PIRA)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRA, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		Or (^^PIRA, ^^IREN, ^^PIRA)
	}
}

Device (LNKB)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 2)

	Name (_PRS, ResourceTemplate ()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 11, 12, 14, 15 }
	})

	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLA, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRB, ^^IREM), IRQ0)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Decrement (Local0)
		Store (Local0, ^^PIRB)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRB, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		Or (^^PIRB, ^^IREN, ^^PIRB)
	}
}

Device (LNKC)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 3)

	Name (_PRS, ResourceTemplate ()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 11, 12, 14, 15 }
	})

	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLA, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRC, ^^IREM), IRQ0)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Decrement (Local0)
		Store (Local0, ^^PIRC)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRC, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		Or (^^PIRC, ^^IREN, ^^PIRC)
	}
}

Device (LNKD)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 4)

	Name (_PRS, ResourceTemplate ()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 11, 12, 14, 15 }
	})

	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLA, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRD, ^^IREM), IRQ0)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Decrement (Local0)
		Store (Local0, ^^PIRD)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRD, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		Or (^^PIRD, ^^IREN, ^^PIRD)
	}
}

Device (LNKE)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 5)

	Name (_PRS, ResourceTemplate ()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 11, 12, 14, 15 }
	})

	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLA, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRE, ^^IREM), IRQ0)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Decrement (Local0)
		Store (Local0, ^^PIRE)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRE, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		Or (^^PIRE, ^^IREN, ^^PIRE)
	}
}

Device (LNKF)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 6)

	Name (_PRS, ResourceTemplate ()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 11, 12, 14, 15 }
	})

	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLA, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRF, ^^IREM), IRQ0)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Decrement (Local0)
		Store (Local0, ^^PIRF)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRF, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		Or (^^PIRF, ^^IREN, ^^PIRF)
	}
}

Device (LNKG)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 7)

	Name (_PRS, ResourceTemplate ()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 11, 12, 14, 15 }
	})

	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLA, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRG, ^^IREM), IRQ0)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Decrement (Local0)
		Store (Local0, ^^PIRG)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRG, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		Or (^^PIRG, ^^IREN, ^^PIRG)
	}
}

Device (LNKH)
{
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 8)

	Name (_PRS, ResourceTemplate ()
	{
		IRQ (Level, ActiveLow, Shared)
			{ 3, 4, 5, 6, 10, 11, 12, 14, 15 }
	})

	Method (_CRS, 0, Serialized)
	{
		Name (RTLA, ResourceTemplate ()
		{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateWordField (RTLA, 1, IRQ0)
		Store (Zero, IRQ0)

		/* Set the bit from PIRQ Routing Register */
		ShiftLeft (1, And (^^PIRH, ^^IREM), IRQ0)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Decrement (Local0)
		Store (Local0, ^^PIRH)
	}

	Method (_STA, 0, Serialized)
	{
		If (And (^^PIRH, ^^IREN)) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		Or (^^PIRH, ^^IREN, ^^PIRH)
	}
}
