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
 */

#include <arch/ioapic.h>

Name(_HID,EISAID("PNP0A08"))	// PCIe
Name(_CID,EISAID("PNP0A03"))	// PCI

Name(_ADR, 0)
Name(_BBN, 0)

// This is in the SSDT and can be accessed by the DSDT
External (BMBD)

// Current Resource Settings
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

	// OPROM reserved (0xd0000-0xd3fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000d0000, 0x000d3fff, 0x00000000,
			0x00004000,,, OPR0)

	// OPROM reserved (0xd4000-0xd7fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000d4000, 0x000d7fff, 0x00000000,
			0x00004000,,, OPR1)

	// OPROM reserved (0xd8000-0xdbfff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000d8000, 0x000dbfff, 0x00000000,
			0x00004000,,, OPR2)

	// OPROM reserved (0xdc000-0xdffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000dc000, 0x000dffff, 0x00000000,
			0x00004000,,, OPR3)

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

	// PCI Memory Region (Top of memory-CONFIG_MMCONF_BASE_ADDRESS)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x00000000, 0, 0x00000000,
			0,,, PM01)

	// TPM Area (0xfed40000-0xfed44fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0xfed40000, 0xfed44fff, 0x00000000,
			0x00005000,,, TPMR)
})	// End MCRS

Method (_CRS, 0, Serialized)
{
	// Find PCI resource area in MCRS
	CreateDwordField(MCRS, ^PM01._MIN, PMIN)
	CreateDwordField(MCRS, ^PM01._MAX, PMAX)
	CreateDwordField(MCRS, ^PM01._LEN, PLEN)

	// Fix up PCI memory region
	// Start with Top of Lower Usable DRAM
	// Memory goes from BMBOUND to CONFIG_MMCONF_BASE_ADDRESS (PM01 above)
	Store (BMBD, PMIN)
	Store (Subtract(CONFIG_MMCONF_BASE_ADDRESS, 1), PMAX)
	Add(Subtract(PMAX, PMIN), 1, PLEN)	// Store Memory Size

	Return (MCRS)
}	// End _CRS
