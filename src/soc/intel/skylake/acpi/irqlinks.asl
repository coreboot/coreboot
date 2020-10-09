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
		 PCR_ITSS_PIRQA_ROUT + CONFIG_PCR_BASE_ADDRESS + (PID_ITSS << PCR_PORTID_SHIFT),
		8)
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
		IRQ0 = 0

		/* Set the bit from PIRQ Routing Register */
		IRQ0 = 1 << (^^PIRA & ^^IREM)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Local0--
		^^PIRA = Local0
	}

	Method (_STA, 0, Serialized)
	{
		If (^^PIRA & ^^IREN) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		^^PIRA |= ^^IREN
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
		IRQ0 = 0

		/* Set the bit from PIRQ Routing Register */
		IRQ0 = 1 << (^^PIRB & ^^IREM)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Local0--
		^^PIRB = Local0
	}

	Method (_STA, 0, Serialized)
	{
		If (^^PIRB & ^^IREN) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		^^PIRB |= ^^IREN
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
		IRQ0 = 0

		/* Set the bit from PIRQ Routing Register */
		IRQ0 = 1 << (^^PIRC & ^^IREM)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Local0--
		^^PIRC = Local0
	}

	Method (_STA, 0, Serialized)
	{
		If (^^PIRC & ^^IREN) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		^^PIRC |= ^^IREN
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
		IRQ0 = 0

		/* Set the bit from PIRQ Routing Register */
		IRQ0 = 1 << (^^PIRD & ^^IREM)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Local0--
		^^PIRD = Local0
	}

	Method (_STA, 0, Serialized)
	{
		If (^^PIRD & ^^IREN) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		^^PIRD |= ^^IREN
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
		IRQ0 = 0

		/* Set the bit from PIRQ Routing Register */
		IRQ0 = 1 << (^^PIRE & ^^IREM)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Local0--
		^^PIRE = Local0
	}

	Method (_STA, 0, Serialized)
	{
		If (^^PIRE & ^^IREN) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		^^PIRE |= ^^IREN
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
		IRQ0 = 0

		/* Set the bit from PIRQ Routing Register */
		IRQ0 = 1 << (^^PIRF & ^^IREM)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Local0--
		^^PIRF = Local0
	}

	Method (_STA, 0, Serialized)
	{
		If (^^PIRF & ^^IREN) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		^^PIRF |= ^^IREN
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
		IRQ0 = 0

		/* Set the bit from PIRQ Routing Register */
		IRQ0 = 1 <<  (^^PIRG & ^^IREM)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Local0--
		^^PIRG = Local0
	}

	Method (_STA, 0, Serialized)
	{
		If (^^PIRG & ^^IREN) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		^^PIRG |= ^^IREN
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
		IRQ0 = 0

		/* Set the bit from PIRQ Routing Register */
		IRQ0 = 1 << (^^PIRH & ^^IREM)

		Return (RTLA)
	}

	Method (_SRS, 1, Serialized)
	{
		CreateWordField (Arg0, 1, IRQ0)
		FindSetRightBit (IRQ0, Local0)
		Local0--
		^^PIRH = Local0
	}

	Method (_STA, 0, Serialized)
	{
		If (^^PIRH & ^^IREN) {
			Return (0x9)
		} Else {
			Return (0xb)
		}
	}

	Method (_DIS, 0, Serialized)
	{
		^^PIRH |= ^^IREN
	}
}
