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

#include <arch/ioapic.h>
#include <cpu/x86/lapic_def.h>

//AMD8111 isa

	Device (ISA)
	{
		// lpc 0x00040000
		Method (_ADR, 0, NotSerialized)
		{
			Return (DADD(\_SB.PCI0.SBDN, 0x00010000))
		}

		OperationRegion (PIRY, PCI_Config, 0x51, 0x02) // LPC Decode Registers
		Field (PIRY, ByteAcc, NoLock, Preserve)
		{
			Z000,   2,	 // Parallel Port Range
			 ,   1,
			ECP,    1,	 // ECP Enable
			FDC1,   1,	 // Floppy Drive Controller 1
			FDC2,   1,	 // Floppy Drive Controller 2
			Offset (0x01),
			Z001,   3,	 // Serial Port A Range
			SAEN,   1,	 // Serial Post A Enabled
			Z002,   3,	 // Serial Port B Range
			SBEN,   1	// Serial Post B Enabled
		}

		Device (PIC)
		{
			Name (_HID, EisaId ("PNP0000"))
			Name (_CRS, ResourceTemplate ()
			{
				IO (Decode16, 0x0020, 0x0020, 0x01, 0x02)
				IO (Decode16, 0x00A0, 0x00A0, 0x01, 0x02)
				IRQ (Edge, ActiveHigh, Exclusive) {2}
			})
		}

		Device (DMA1)
		{
			Name (_HID, EisaId ("PNP0200"))
			Name (_CRS, ResourceTemplate ()
			{
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x10)
				IO (Decode16, 0x0080, 0x0080, 0x01, 0x10)
				IO (Decode16, 0x00C0, 0x00C0, 0x01, 0x20)
				DMA (Compatibility, NotBusMaster, Transfer16) {4}
			})
		}

		Device (TMR)
		{
			Name (_HID, EisaId ("PNP0100"))
			Name (_CRS, ResourceTemplate ()
			{
				IO (Decode16, 0x0040, 0x0040, 0x01, 0x04)
				IRQ (Edge, ActiveHigh, Exclusive) {0}
			})
		}

		Device (RTC)
		{
			Name (_HID, EisaId ("PNP0B00"))
			Name (_CRS, ResourceTemplate ()
			{
				IO (Decode16, 0x0070, 0x0070, 0x01, 0x06)
				IRQ (Edge, ActiveHigh, Exclusive) {8}
			})
		}

		Device (SPKR)
		{
			Name (_HID, EisaId ("PNP0800"))
			Name (_CRS, ResourceTemplate ()
			{
				IO (Decode16, 0x0061, 0x0061, 0x01, 0x01)
			})
		}

		Device (COPR)
		{
			Name (_HID, EisaId ("PNP0C04"))
			Name (_CRS, ResourceTemplate ()
			{
				IO (Decode16, 0x00F0, 0x00F0, 0x01, 0x10)
				IRQ (Edge, ActiveHigh, Exclusive) {13}
			})
		}

		Device (SYSR)
		{
			Name (_HID, EisaId ("PNP0C02"))
			Name (_UID, 0x00)
			Name (SYR1, ResourceTemplate ()
			{
				IO (Decode16, 0x04D0, 0x04D0, 0x01, 0x02) //report Thor NVRAM
				IO (Decode16, 0x1100, 0x117F, 0x01, 0x80) //report Thor NVRAM
				IO (Decode16, 0x1180, 0x11FF, 0x01, 0x80)
				IO (Decode16, 0x0010, 0x0010, 0x01, 0x10)
				IO (Decode16, 0x0022, 0x0022, 0x01, 0x1E)
				IO (Decode16, 0x0044, 0x0044, 0x01, 0x1C)
				IO (Decode16, 0x0062, 0x0062, 0x01, 0x02)
				IO (Decode16, 0x0065, 0x0065, 0x01, 0x0B)
				IO (Decode16, 0x0076, 0x0076, 0x01, 0x0A)
				IO (Decode16, 0x0090, 0x0090, 0x01, 0x10)
				IO (Decode16, 0x00A2, 0x00A2, 0x01, 0x1E)
				IO (Decode16, 0x00E0, 0x00E0, 0x01, 0x10)
				IO (Decode16, 0x0B78, 0x0B78, 0x01, 0x04) // Added this to remove ACPI Unrepoted IO Error
				IO (Decode16, 0x0190, 0x0190, 0x01, 0x04) // Added this to remove ACPI Unrepoted IO Error
				})
			Method (_CRS, 0, NotSerialized)
			{
				Return (SYR1)
			}
		}

		Device (MEM)
		{
			Name (_HID, EisaId ("PNP0C02"))
			Name (_UID, 0x01)
			Method (_CRS, 0, NotSerialized)
			{
				Name (BUF0, ResourceTemplate ()
				{
					Memory32Fixed (ReadWrite, 0x000E0000, 0x00020000) // BIOS E4000-FFFFF
					Memory32Fixed (ReadWrite, 0x000C0000, 0x00010000) // video BIOS c0000-c8404
					Memory32Fixed (ReadWrite, IO_APIC_ADDR, 0x00001000)
					Memory32Fixed (ReadWrite, 0xFFC00000, 0x00380000) // LPC forwarded, 4 MB w/ROM
					Memory32Fixed (ReadWrite, LOCAL_APIC_ADDR, 0x00001000)
					Memory32Fixed (ReadWrite, 0xFFF80000, 0x00080000) // Overlay BIOS
					Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
					Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
					Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
					Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
				})
				// Read the Video Memory length
				CreateDWordField (BUF0, 0x14, CLEN)
				CreateDWordField (BUF0, 0x10, CBAS)

				ShiftLeft (VGA1, 0x09, Local0)
				Store (Local0, CLEN)

				Return (BUF0)
			}
		}

		Device (PS2M)
		{
			Name (_HID, EisaId ("PNP0F13"))
			Name (_CRS, ResourceTemplate ()
			{
				IRQNoFlags () {12}
			})
			Method (_STA, 0, NotSerialized)
			{
				And (FLG0, 0x04, Local0)
				If (LEqual (Local0, 0x04)) { Return (0x0F) }
				Else { Return (0x00) }
			}
		}

		Device (PS2K)
		{
			Name (_HID, EisaId ("PNP0303"))
			Name (_CRS, ResourceTemplate ()
			{
				IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
				IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
				IRQNoFlags () {1}
			})
		}
		#include "superio.asl"

	}

