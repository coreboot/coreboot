/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/ioapic.h>

Name(_HID,EISAID("PNP0A08"))	// PCIe
Name(_CID,EISAID("PNP0A03"))	// PCI

Name(_ADR, 0)
Name(_BBN, 0)

Device (MCHC)
{
	Name(_ADR, 0x00000000)	// 0:0.0

	OperationRegion(MCHP, PCI_Config, 0x00, 0x100)
	Field (MCHP, DWordAcc, NoLock, Preserve)
	{
		Offset (0x40),	// EPBAR
		EPEN,	 1,	// Enable
		,	11,	//
		EPBR,	20,	// EPBAR

		Offset (0x44),	// MCHBAR
		MHEN,	 1,	// Enable
		,	13,	//
		MHBR,	18,	// MCHBAR

		Offset (0x48),	// PCIe BAR
		PXEN,	 1,	// Enable
		PXSZ,	 2,	// BAR size
		,	23,	//
		PXBR,	 6,	// PCIe BAR

		Offset (0x4c),	// DMIBAR
		DMEN,	 1,	// Enable
		,	11,	//
		DMBR,	20,	// DMIBAR

		// ...

		Offset (0x90),	// PAM0
		,	 4,
		PM0H,	 2,
		,	 2,
		Offset (0x91),	// PAM1
		PM1L,	 2,
		,	 2,
		PM1H,	 2,
		,	 2,
		Offset (0x92),	// PAM2
		PM2L,	 2,
		,	 2,
		PM2H,	 2,
		,	 2,
		Offset (0x93),	// PAM3
		PM3L,	 2,
		,	 2,
		PM3H,	 2,
		,	 2,
		Offset (0x94),	// PAM4
		PM4L,	 2,
		,	 2,
		PM4H,	 2,
		,	 2,
		Offset (0x95),	// PAM5
		PM5L,	 2,
		,	 2,
		PM5H,	 2,
		,	 2,
		Offset (0x96),	// PAM6
		PM6L,	 2,
		,	 2,
		PM6H,	 2,
		,	 2,

		Offset (0x9c),	// Top of Low Used Memory
		,	 3,
		TLUD,	 5,

		Offset (0xa0),	// Top of Used Memory
		TOM,	16,
	}

}


// Current Resource Settings

Method (_CRS, 0, Serialized)
{
	Name (MCRS, ResourceTemplate()
	{
		// Bus Numbers
		WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
				0x0000, 0x0000, 0x00ff, 0x0000, 0x0100,,, PB00)

		// IO Region 0
		DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
				0x0000, 0x0000, 0x0cf7, 0x0000, 0x0cf8,,, PI00)

		// PCI Config Space
		Io (Decode16, 0x0cf8, 0x0cf8, 0x0001, 0x0008)

		// IO Region 1
		DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
				0x0000, 0x0d00, 0xffff, 0x0000, 0xf300,,, PI01)

		// VGA memory (0xa0000-0xbffff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000a0000, 0x000bffff, 0x00000000,
				0x00020000,,, ASEG)

		// OPROM reserved (0xc0000-0xc3fff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000c0000, 0x000c3fff, 0x00000000,
				0x00004000,,, OPR0)

		// OPROM reserved (0xc4000-0xc7fff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000c4000, 0x000c7fff, 0x00000000,
				0x00004000,,, OPR1)

		// OPROM reserved (0xc8000-0xcbfff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000c8000, 0x000cbfff, 0x00000000,
				0x00004000,,, OPR2)

		// OPROM reserved (0xcc000-0xcffff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000cc000, 0x000cffff, 0x00000000,
				0x00004000,,, OPR3)

		// OPROM reserved (0xd0000-0xd3fff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000d0000, 0x000d3fff, 0x00000000,
				0x00004000,,, OPR4)

		// OPROM reserved (0xd4000-0xd7fff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000d4000, 0x000d7fff, 0x00000000,
				0x00004000,,, OPR5)

		// OPROM reserved (0xd8000-0xdbfff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000d8000, 0x000dbfff, 0x00000000,
				0x00004000,,, OPR6)

		// OPROM reserved (0xdc000-0xdffff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000dc000, 0x000dffff, 0x00000000,
				0x00004000,,, OPR7)

		// BIOS Extension (0xe0000-0xe3fff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000e0000, 0x000e3fff, 0x00000000,
				0x00004000,,, ESG0)

		// BIOS Extension (0xe4000-0xe7fff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000e4000, 0x000e7fff, 0x00000000,
				0x00004000,,, ESG1)

		// BIOS Extension (0xe8000-0xebfff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000e8000, 0x000ebfff, 0x00000000,
				0x00004000,,, ESG2)

		// BIOS Extension (0xec000-0xeffff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000ec000, 0x000effff, 0x00000000,
				0x00004000,,, ESG3)

		// System BIOS (0xf0000-0xfffff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000f0000, 0x000fffff, 0x00000000,
				0x00010000,,, FSEG)

		// PCI Memory Region (Top of memory-0xfebfffff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x00000000, 0xfebfffff, 0x00000000,
				IO_APIC_ADDR,,, PM01)

		// TPM Area (0xfed40000-0xfed44fff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0xfed40000, 0xfed44fff, 0x00000000,
				0x00005000,,, TPMR)
	})

	// Find PCI resource area in MCRS
	CreateDwordField(MCRS, PM01._MIN, PMIN)
	CreateDwordField(MCRS, PM01._MAX, PMAX)
	CreateDwordField(MCRS, PM01._LEN, PLEN)

	// Fix up PCI memory region:
	// Enter actual TOLUD. The TOLUD register contains bits 27-31 of
	// the top of memory address.
	ShiftLeft (^MCHC.TLUD, 27, PMIN)
	Add(Subtract(PMAX, PMIN), 1, PLEN)

	Return (MCRS)
}

/* IRQ assignment is mainboard specific. Get it from mainboard ACPI code */
#include "acpi/i945_pci_irqs.asl"


