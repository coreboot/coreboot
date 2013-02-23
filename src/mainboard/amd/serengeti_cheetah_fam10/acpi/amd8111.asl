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

//AMD8111
	Name (APIC, Package (0x04)
	{
		Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x10},// 0x0004ffff : assusme 8131 is present
		Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x11},
		Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x12},
		Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x13}
	})

	Name (PICM, Package (0x04)
	{
		Package (0x04) { 0x0004FFFF, 0x00, \_SB.PCI0.LNKA, 0x00},
		Package (0x04) { 0x0004FFFF, 0x01, \_SB.PCI0.LNKB, 0x00},
		Package (0x04) { 0x0004FFFF, 0x02, \_SB.PCI0.LNKC, 0x00},
		Package (0x04) { 0x0004FFFF, 0x03, \_SB.PCI0.LNKD, 0x00}
	})

	Name (DNCG, Ones)

	Method (_PRT, 0, NotSerialized)
	{
		If (LEqual (^DNCG, Ones)) {
			Store (DADD(\_SB.PCI0.SBDN, 0x0001ffff), Local0)
			// Update the Device Number according to SBDN
			Store(Local0, Index (DeRefOf (Index (PICM, 0)), 0))
			Store(Local0, Index (DeRefOf (Index (PICM, 1)), 0))
			Store(Local0, Index (DeRefOf (Index (PICM, 2)), 0))
			Store(Local0, Index (DeRefOf (Index (PICM, 3)), 0))
			Store(Local0, Index (DeRefOf (Index (APIC, 0)), 0))
			Store(Local0, Index (DeRefOf (Index (APIC, 1)), 0))
			Store(Local0, Index (DeRefOf (Index (APIC, 2)), 0))
			Store(Local0, Index (DeRefOf (Index (APIC, 3)), 0))
			Store (0x00, ^DNCG)
		}
		If (LNot (PICF)) {Return (PICM)}
		Else {Return (APIC)}
	}

	Device (SBC3)
	{
		// acpi smbus it should be 0x00040003 if 8131 present
		Method (_ADR, 0, NotSerialized)
		{
			Return (DADD(\_SB.PCI0.SBDN, 0x00010003))
		}
		OperationRegion (PIRQ, PCI_Config, 0x56, 0x02)
		Field (PIRQ, ByteAcc, Lock, Preserve)
		{
			PIBA,   8,
			PIDC,   8
		}
//
//		OperationRegion (TS3_, PCI_Config, 0xC4, 0x02)
//		Field (TS3_, DWordAcc, NoLock, Preserve)
//		{
//			PTS3,   16
//		}
//
	}

	Device (HPET)
	{
		Name (HPT, 0x00)
		Name (_HID, EisaId ("PNP0103"))
		Name (_UID, 0x00)
		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}

		Method (_CRS, 0, NotSerialized)
		{
			Name (BUF0, ResourceTemplate ()
			{
				Memory32Fixed (ReadWrite, 0xFED00000, 0x00000400)
			})
			Return (BUF0)
		}
	}

	#include "amd8111_pic.asl"

	#include "amd8111_isa.asl"

	Device (TP2P)
	{
		// 8111 P2P and it should 0x00030000 when 8131 present
		Method (_ADR, 0, NotSerialized)
		{
			Return (DADD(\_SB.PCI0.SBDN, 0x00000000))
		}

		Method (_PRW, 0, NotSerialized)
		{
			If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x08, 0x03 }) }
			Else { Return (Package (0x02) { 0x08, 0x01 }) }
		}

		Device (USB0)
		{
			Name (_ADR, 0x00000000)
			Method (_PRW, 0, NotSerialized)
			{
				If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x0F, 0x03 }) }
				Else { Return (Package (0x02) { 0x0F, 0x01 }) }
			}
		}

		Device (USB1)
		{
			Name (_ADR, 0x00000001)
			Method (_PRW, 0, NotSerialized)
			{
				If (CondRefOf (\_S3, Local0)) { Return (Package (0x02) { 0x0F, 0x03 }) }
				Else { Return (Package (0x02) { 0x0F, 0x01 }) }
			}
		}

		Name (APIC, Package (0x0C)
		{
			Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x10 }, //USB
			Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x11 },
			Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x12 },
			Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x13 },

			Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x10 }, //Slot 4
			Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x11 },
			Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x12 },
			Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x13 },

			Package (0x04) { 0x0005FFFF, 0x00, 0x00, 0x11 }, //Slot 3
			Package (0x04) { 0x0005FFFF, 0x01, 0x00, 0x12 },
			Package (0x04) { 0x0005FFFF, 0x02, 0x00, 0x13 },
			Package (0x04) { 0x0005FFFF, 0x03, 0x00, 0x10 }
		})

		Name (PICM, Package (0x0C)
		{
			Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 }, //USB
			Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
			Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
			Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },

			Package (0x04) { 0x0004FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 }, //Slot 4
			Package (0x04) { 0x0004FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
			Package (0x04) { 0x0004FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
			Package (0x04) { 0x0004FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },

			Package (0x04) { 0x0005FFFF, 0x00, \_SB.PCI0.LNKB, 0x00 }, //Slot 3
			Package (0x04) { 0x0005FFFF, 0x01, \_SB.PCI0.LNKC, 0x00 },
			Package (0x04) { 0x0005FFFF, 0x02, \_SB.PCI0.LNKD, 0x00 },
			Package (0x04) { 0x0005FFFF, 0x03, \_SB.PCI0.LNKA, 0x00 }
		})

		Method (_PRT, 0, NotSerialized)
		{
			If (LNot (PICF)) { Return (PICM) }
			Else { Return (APIC) }
		}
	}

