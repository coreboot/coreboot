/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <soc/intel/baytrail/baytrail/iomap.h>
#include <soc/intel/baytrail/baytrail/irq.h>

Scope(\)
{
	// IO-Trap at 0x800. This is the ACPI->SMI communication interface.

	OperationRegion(IO_T, SystemIO, 0x800, 0x10)
	Field(IO_T, ByteAcc, NoLock, Preserve)
	{
		Offset(0x8),
		TRP0, 8		// IO-Trap at 0x808
	}

	// Intel Legacy Block
	OperationRegion(ILBS, SystemMemory, ILB_BASE_ADDRESS, ILB_BASE_SIZE)
	Field (ILBS, AnyAcc, NoLock, Preserve)
	{
		Offset (0x8),
		PRTA, 8,
		PRTB, 8,
		PRTC, 8,
		PRTD, 8,
		PRTE, 8,
		PRTF, 8,
		PRTG, 8,
		PRTH, 8,
	}
}

Name(_HID,EISAID("PNP0A08"))	// PCIe
Name(_CID,EISAID("PNP0A03"))	// PCI

Name(_ADR, 0)
Name(_BBN, 0)

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

		// PCI Memory Region (Top of memory-0xfeafffff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0xfea00000, 0xfeafffff, 0x00000000,
				0x00100000,,, PMEM)

		// TPM Area (0xfed40000-0xfed44fff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0xfed40000, 0xfed44fff, 0x00000000,
				0x00005000,,, TPMR)
	})

	// Update PCI resource area
	CreateDwordField(MCRS, PMEM._MIN, PMIN)
	CreateDwordField(MCRS, PMEM._MAX, PMAX)
	CreateDwordField(MCRS, PMEM._LEN, PLEN)

	// TOLM is BMBOUND accessible from IOSF so is saved in NVS
	Store (\TOLM, PMIN)
	Add (Subtract (PMAX, PMIN), 1, PLEN)

	Return (MCRS)
}

/* Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 1)

	Name (PDRS, ResourceTemplate() {
		Memory32Fixed(ReadWrite, ABORT_BASE_ADDRESS, ABORT_BASE_SIZE)
		Memory32Fixed(ReadWrite, MCFG_BASE_ADDRESS, MCFG_BASE_SIZE)
		Memory32Fixed(ReadWrite, PMC_BASE_ADDRESS, PMC_BASE_SIZE)
		Memory32Fixed(ReadWrite, ILB_BASE_ADDRESS, ILB_BASE_SIZE)
		Memory32Fixed(ReadWrite, SPI_BASE_ADDRESS, SPI_BASE_SIZE)
		Memory32Fixed(ReadWrite, MPHY_BASE_ADDRESS, MPHY_BASE_SIZE)
		Memory32Fixed(ReadWrite, PUNIT_BASE_ADDRESS, PUNIT_BASE_SIZE)
		Memory32Fixed(ReadWrite, RCBA_BASE_ADDRESS, RCBA_BASE_SIZE)
#if CONFIG_CHROMEOS_RAMOOPS
		Memory32Fixed(ReadWrite, CONFIG_CHROMEOS_RAMOOPS_RAM_START,
					 CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE)
#endif
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		Return(PDRS)
	}
}

Method (_OSC, 4)
{
	/* Check for proper GUID */
	If (LEqual (Arg0, ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")))
	{
		/* Let OS control everything */
		Return (Arg3)
	}
	Else
	{
		/* Unrecognized UUID */
		CreateDWordField (Arg3, 0, CDW1)
		Or (CDW1, 4, CDW1)
		Return (Arg3)
	}
}

/* IOSF MBI Interface for kernel access */
Device (IOSF)
{
	Name (_HID, "INT33BD")
	Name (_CID, "INT33BD")
	Name (_UID, 1)

	Name (RBUF, ResourceTemplate ()
	{
		/* MCR / MDR / MCRX */
		Memory32Fixed (ReadWrite, 0, 12, RBAR)
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^RBAR._BAS, RBAS)
		Store (Add (MCFG_BASE_ADDRESS, 0xD0), RBAS)
		Return (^RBUF)
	}
}

// LPC Bridge 0:1f.0
#include "lpc.asl"

// IRQ routing for each PCI device
#include "irqroute.asl"

Scope (\_SB)
{
	// GPIO Devices
	#include "gpio.asl"

	// LPSS Devices
	#include "lpss.asl"

	// SCC Devices
	#include "scc.asl"
}
