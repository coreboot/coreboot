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
DefinitionBlock ("DSDT.aml", "DSDT", 1, "AMDF10", "AMDACPI ", 100925440)
{

//	Scope (_PR)
//	{
//		Processor (CPU0, 0x00, 0x0000C010, 0x06) {}
//		Processor (CPU1, 0x01, 0x00000000, 0x00) {}
//		Processor (CPU2, 0x02, 0x00000000, 0x00) {}
//		Processor (CPU3, 0x03, 0x00000000, 0x00) {}
//	}

	Method (FWSO, 0, NotSerialized) { }


	Scope (_SB)
	{
		Device (PCI0)
		{
			// BUS0 root bus
			External (BUSN)
			External (MMIO)
			External (PCIO)
			External (SBLK)
			External (TOM1)
			External (HCLK)
			External (SBDN)
			External (HCDN)
			External (CBST)
			External (CBB)
			External (CBS2)
			External (CBB2)

			Name (_HID, EisaId ("PNP0A03"))
			Name (_ADR, 0x00000000)
			Name (_UID, 0x01)

			Name (HCIN, 0x00)  // HC1

			Method (_BBN, 0, NotSerialized)
			{
				Return (GBUS (GHCN(HCIN), GHCL(HCIN)))
			}

			Method (_CRS, 0, NotSerialized)
			{
				Name (BUF0, ResourceTemplate ()
				{
					IO (Decode16, 0x0CF8, 0x0CF8, 0x01, 0x08) //CF8-CFFh
					IO (Decode16, 0xC000, 0xC000, 0x01, 0x80) //8000h
					IO (Decode16, 0xC080, 0xC080, 0x01, 0x80) //8080h

					WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
						0x0000, // Address Space Granularity
						0x8100, // Address Range Minimum
						0xFFFF, // Address Range Maximum
						0x0000, // Address Translation Offset
						0x7F00,,,
						, TypeStatic)	  //8100h-FFFFh

					DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
						0x00000000, // Address Space Granularity
						0x000C0000, // Address Range Minimum
						0x000CFFFF, // Address Range Maximum
						0x00000000, // Address Translation Offset
						0x00010000,,,
						, AddressRangeMemory, TypeStatic)   //Video BIOS A0000h-C7FFFh

					Memory32Fixed (ReadWrite, 0x000D8000, 0x00004000)//USB HC D8000-DBFFF

					WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
						0x0000, // Address Space Granularity
						0x0000, // Address Range Minimum
						0x03AF, // Address Range Maximum
						0x0000, // Address Translation Offset
						0x03B0,,,
						, TypeStatic)	//0-CF7h

					WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
						0x0000, // Address Space Granularity
						0x03E0, // Address Range Minimum
						0x0CF7, // Address Range Maximum
						0x0000, // Address Translation Offset
						0x0918,,,
						, TypeStatic)	//0-CF7h
				})
				\_SB.OSVR ()
				CreateDWordField (BUF0, 0x3E, VLEN)
				CreateDWordField (BUF0, 0x36, VMAX)
				CreateDWordField (BUF0, 0x32, VMIN)
				ShiftLeft (VGA1, 0x09, Local0)
				Add (VMIN, Local0, VMAX)
				Decrement (VMAX)
				Store (Local0, VLEN)
				Concatenate (\_SB.GMEM (0x00, \_SB.PCI0.SBLK), BUF0, Local1)
				Concatenate (\_SB.GIOR (0x00, \_SB.PCI0.SBLK), Local1, Local2)
				Concatenate (\_SB.GWBN (0x00, \_SB.PCI0.SBLK), Local2, Local3)
				Return (Local3)
			}

			#include "acpi/pci0_hc.asl"

		}
		Device (PCI1)
		{
			Name (_HID, "PNP0A03")
			Name (_ADR, 0x00000000)
			Name (_UID, 0x02)
			Method (_STA, 0, NotSerialized)
			{
				Return (\_SB.PCI0.CBST)
			}
			Method (_BBN, 0, NotSerialized)
			{
				Return (\_SB.PCI0.CBB) // 0 or 0xff
			}

		}
		Device (PCI2)
		{
			Name (_HID, "PNP0A03")
			Name (_ADR, 0x00000000)
			Name (_UID, 0x02)
			Method (_STA, 0, NotSerialized)
			{
				Return (\_SB.PCI0.CBS2)
			}
			Method (_BBN, 0, NotSerialized)
			{
				Return (\_SB.PCI0.CBB2)// 0xfe
			}
		}
	}

	Scope (_GPE)
	{
		Method (_L08, 0, NotSerialized)
		{
			Notify (\_SB.PCI0, 0x02) //PME# Wakeup
		}

		Method (_L0F, 0, NotSerialized)
		{
			Notify (\_SB.PCI0.TP2P.USB0, 0x02)	 //USB Wakeup
		}

		Method (_L22, 0, NotSerialized) // GPIO18 (LID) - Pogo 0 Bridge B
		{
			Notify (\_SB.PCI0.PG0B, 0x02)
		}

		Method (_L29, 0, NotSerialized) // GPIO25 (Suspend) - Pogo 0 Bridge A
		{
			Notify (\_SB.PCI0.PG0A, 0x02)
		}
	}

	Method (_PTS, 1, NotSerialized)
	{
		Or (Arg0, 0xF0, Local0)
		Store (Local0, DBG1)
	}
//
//	Method (_WAK, 1, NotSerialized)
//	{
//		Or (Arg0, 0xE0, Local0)
//		Store (Local0, DBG1)
//	}

	Name (PICF, 0x00) //Flag Variable for PIC vs. I/O APIC Mode
	Method (_PIC, 1, NotSerialized) //PIC Flag and Interface Method
	{
		Store (Arg0, PICF)
	}

	OperationRegion (DEBG, SystemIO, 0x80, 0x01)
	Field (DEBG, ByteAcc, Lock, Preserve)
	{
		DBG1, 8
	}

	OperationRegion (EXTM, SystemMemory, 0x000FF83C, 0x04)
	Field (EXTM, WordAcc, Lock, Preserve)
	{
		AMEM, 32
	}

	OperationRegion (VGAM, SystemMemory, 0x000C0002, 0x01)
	Field (VGAM, ByteAcc, Lock, Preserve)
	{
		VGA1, 8
	}

	OperationRegion (GRAM, SystemMemory, 0x0400, 0x0100)
	Field (GRAM, ByteAcc, Lock, Preserve)
	{
		Offset (0x10),
		FLG0, 8
	}

	OperationRegion (GSTS, SystemIO, 0xC028, 0x02)
	Field (GSTS, ByteAcc, NoLock, Preserve)
	{
		, 4,
		IRQR, 1
	}

	OperationRegion (Z007, SystemIO, 0x21, 0x01)
	Field (Z007, ByteAcc, NoLock, Preserve)
	{
		Z008, 8
	}

	OperationRegion (Z009, SystemIO, 0xA1, 0x01)
	Field (Z009, ByteAcc, NoLock, Preserve)
	{
		Z00A, 8
	}

	#include <northbridge/amd/amdfam10/amdfam10_util.asl>
}
