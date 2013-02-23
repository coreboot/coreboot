//
// This file is part of the coreboot project.
//
// Copyright (C) 2007 Advanced Micro Devices, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//

	Device (PG0A)
	{
		// 8132 pcix bridge
		Method (_ADR, 0, NotSerialized)
		{
			Return (DADD(GHCD(HCIN, 0), 0x00000000))
		}

		Method (_PRW, 0, NotSerialized)
		{
			If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x29, 0x03 }) }
			Else { Return (Package (0x02) { 0x29, 0x01 }) }
		}

		Name (APIC, Package (0x10)
		{
		// Slot 1 - PIRQ ABCD
			Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x0018 },
			Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x0019 },
			Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x001A },
			Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x001B },
			// Slot 2 - PIRQ BCDA
			Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x0019 },
			Package (0x04) { 0x0001FFFF, 0x01, 0x00, 0x001A },
			Package (0x04) { 0x0001FFFF, 0x02, 0x00, 0x001B },
			Package (0x04) { 0x0001FFFF, 0x03, 0x00, 0x0018 },
			// Slot 3 - PIRQ CDAB
			Package (0x04) { 0x0002FFFF, 0x00, 0x00, 0x001A },
			Package (0x04) { 0x0002FFFF, 0x01, 0x00, 0x001B },
			Package (0x04) { 0x0002FFFF, 0x02, 0x00, 0x0018 },
			Package (0x04) { 0x0002FFFF, 0x03, 0x00, 0x0019 },
			// Slot 4 - PIRQ DABC
			Package (0x04) { 0x0003FFFF, 0x00, 0x00, 0x001B },
			Package (0x04) { 0x0003FFFF, 0x01, 0x00, 0x0018 },
			Package (0x04) { 0x0003FFFF, 0x02, 0x00, 0x0019 },
			Package (0x04) { 0x0003FFFF, 0x03, 0x00, 0x001A },

		})
		Name (PICM, Package (0x04)
		{
			Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },//Slot 2
			Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
			Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
			Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },
//
//			Package (0x04) { 0x0001FFFF, 0x00, \_SB.PCI0.LNKB, 0x00 },//Slot 2
//			Package (0x04) { 0x0001FFFF, 0x01, \_SB.PCI0.LNKC, 0x00 },
//			Package (0x04) { 0x0001FFFF, 0x02, \_SB.PCI0.LNKD, 0x00 },
//			Package (0x04) { 0x0001FFFF, 0x03, \_SB.PCI0.LNKA, 0x00 },
//
//			Package (0x04) { 0x0002FFFF, 0x00, \_SB.PCI0.LNKC, 0x00 },//Slot 2
//			Package (0x04) { 0x0002FFFF, 0x01, \_SB.PCI0.LNKD, 0x00 },
//			Package (0x04) { 0x0002FFFF, 0x02, \_SB.PCI0.LNKA, 0x00 },
//			Package (0x04) { 0x0002FFFF, 0x03, \_SB.PCI0.LNKB, 0x00 },
//
//			Package (0x04) { 0x0003FFFF, 0x00, \_SB.PCI0.LNKD, 0x00 },//Slot 2
//			Package (0x04) { 0x0003FFFF, 0x01, \_SB.PCI0.LNKA, 0x00 },
//			Package (0x04) { 0x0003FFFF, 0x02, \_SB.PCI0.LNKB, 0x00 },
//			Package (0x04) { 0x0003FFFF, 0x03, \_SB.PCI0.LNKC, 0x00 },
//
		})

		Name (DNCG, Ones)

		Method (_PRT, 0, NotSerialized)
		{
			If (LEqual (^DNCG, Ones)) {
				Multiply (HCIN, 0x000e, Local2) // GSI for 8132 is 7 so we get 14
				Store (0x00, Local1)
				While (LLess (Local1, 0x10))
				{
					// Update the GSI according to HCIN
					Store(DeRefOf(Index (DeRefOf (Index (APIC, Local1)), 3)), Local0)
					Add(Local2, Local0, Local0)
					Store(Local0, Index (DeRefOf (Index (APIC, Local1)), 3))
					Increment (Local1)
				}

				Store (0x00, ^DNCG)

			}

			If (LNot (PICF)) { Return (PICM) }
			Else { Return (APIC) }
		}
	}

	Device (PG0B)
	{
		// 8132 pcix bridge 2
		Method (_ADR, 0, NotSerialized)
		{
			Return (DADD(GHCD(HCIN, 0), 0x00010000))
		}

		Method (_PRW, 0, NotSerialized)
		{
			If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x22, 0x03 }) }
			Else { Return (Package (0x02) { 0x22, 0x01 }) }
		}

		Name (APIC, Package (0x10)
		{
			// Slot A - PIRQ ABCD
			Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x001F },// Slot 1
			Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x0020 },
			Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x0021 },
			Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x0022 },
			// Slot A - PIRQ BCDA
			Package (0x04) { 0x0001FFFF, 0x00, 0x00, 0x0020 },// Slot 1
			Package (0x04) { 0x0001FFFF, 0x01, 0x00, 0x0021 },
			Package (0x04) { 0x0001FFFF, 0x02, 0x00, 0x0022 },
			Package (0x04) { 0x0001FFFF, 0x03, 0x00, 0x001F },
			// Slot A - PIRQ CDAB
			Package (0x04) { 0x0002FFFF, 0x00, 0x00, 0x0021 },// Slot 1
			Package (0x04) { 0x0002FFFF, 0x01, 0x00, 0x0022 },
			Package (0x04) { 0x0002FFFF, 0x02, 0x00, 0x001F },
			Package (0x04) { 0x0002FFFF, 0x03, 0x00, 0x0020 },
			// Slot A - PIRQ DABC
			Package (0x04) { 0x0003FFFF, 0x00, 0x00, 0x0022 },// Slot 1
			Package (0x04) { 0x0003FFFF, 0x01, 0x00, 0x001F },
			Package (0x04) { 0x0003FFFF, 0x02, 0x00, 0x0020 },
			Package (0x04) { 0x0003FFFF, 0x03, 0x00, 0x0021 },
		})
		Name (PICM, Package (0x04)
		{
			Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },//Slot 1
			Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
			Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
			Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },

//			Package (0x04) { 0x0001FFFF, 0x00, \_SB.PCI0.LNKB, 0x00 },//Slot 1
//			Package (0x04) { 0x0001FFFF, 0x01, \_SB.PCI0.LNKC, 0x00 },
//			Package (0x04) { 0x0001FFFF, 0x02, \_SB.PCI0.LNKD, 0x00 },
//			Package (0x04) { 0x0001FFFF, 0x03, \_SB.PCI0.LNKA, 0x00 },
//
//			Package (0x04) { 0x0002FFFF, 0x00, \_SB.PCI0.LNKC, 0x00 },//Slot 1
//			Package (0x04) { 0x0002FFFF, 0x01, \_SB.PCI0.LNKD, 0x00 },
//			Package (0x04) { 0x0002FFFF, 0x02, \_SB.PCI0.LNKA, 0x00 },
//			Package (0x04) { 0x0002FFFF, 0x03, \_SB.PCI0.LNKB, 0x00 },
//
//			Package (0x04) { 0x0003FFFF, 0x00, \_SB.PCI0.LNKD, 0x00 },//Slot 1
//			Package (0x04) { 0x0003FFFF, 0x01, \_SB.PCI0.LNKA, 0x00 },
//			Package (0x04) { 0x0003FFFF, 0x02, \_SB.PCI0.LNKB, 0x00 },
//			Package (0x04) { 0x0003FFFF, 0x03, \_SB.PCI0.LNKC, 0x00 },
//
		})

		Name (DNCG, Ones)

		Method (_PRT, 0, NotSerialized)
		{
			If (LEqual (^DNCG, Ones)) {
				Multiply (HCIN, 0x000e, Local2) // GSI for 8132 is 7 so we get 14
				Store (0x00, Local1)
				While (LLess (Local1, 0x10))
				{
					// Update the GSI according to HCIN
					Store(DeRefOf(Index (DeRefOf (Index (APIC, Local1)), 3)), Local0)
					Add(Local2, Local0, Local0)
					Store(Local0, Index (DeRefOf (Index (APIC, Local1)), 3))
					Increment (Local1)
				}

			Store (0x00, ^DNCG)

			}

			If (LNot (PICF)) { Return (PICM) }
			Else { Return (APIC) }
		}
	}
