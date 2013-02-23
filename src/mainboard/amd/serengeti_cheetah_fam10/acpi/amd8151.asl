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

// AMD8151
	Device (AGPB)
	{
		Method (_ADR, 0, NotSerialized)
		{
			Return (DADD(GHCD(HCIN, 0), 0x00010000))
		}

		Name (APIC, Package (0x04)
		{
			Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x10 },
			Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x11 },
			Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x12 },
			Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x13 }
		})
		Name (PICM, Package (0x04)
		{
			Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 },
			Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
			Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
			Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 }
		})
		Method (_PRT, 0, NotSerialized)
		{
			If (LNot (PICF)) { Return (PICM) }
			Else { Return (APIC) }
		}
	}

