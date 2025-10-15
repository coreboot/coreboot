/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/haswell/memmap.h>

Name (_HID, EISAID ("PNP0A08"))	// PCIe
Name (_CID, EISAID ("PNP0A03"))	// PCI

Name (_BBN, 0)

Device (MCHC)
{
	Name (_ADR, 0x00000000)	// 0:0.0

	OperationRegion (MCHP, PCI_Config, 0x00, 0x100)
	Field (MCHP, DWordAcc, NoLock, Preserve)
	{
		Offset (0x60),	// PCIEXBAR
		PXEN,	 1,	// Enable
		PXSZ,	 2,	// PCIEXBAR size
		,	23,
		PXBR,	10,	// PCIEXBAR base

		Offset (0x70),	// ME Base Address
		MEBA,	 64,
		Offset (0xa0),	// Top of Used Memory
		TOM,	 64,
		Offset (0xbc),	// Top of Low Used Memory
		TLUD,	 32,
	}
}

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

	// PCI Memory Region below MMCONF (TOLUD)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000,,, PM01)
#if CONFIG(SOC_INTEL_BROADWELL)
	// PCI Memory Region above MMCONF (dynamic, based on ECAM size)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000,,, PM02)
#endif
	// TPM Area (0xfed40000-0xfed44fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0xfed40000, 0xfed44fff, 0x00000000,
			0x00005000,,, TPMR)
})

Method (_CRS, 0, Serialized)
{
	// Find PCI resource area in MCRS
	CreateDwordField (MCRS, ^PM01._MIN, PMIN)
	CreateDwordField (MCRS, ^PM01._MAX, PMAX)
	CreateDwordField (MCRS, ^PM01._LEN, PLEN)

	// Fix up PCI memory region
	// Start with Top of Lower Usable DRAM
	// Lower 20 bits of TOLUD register need to be masked since they contain lock and
	// reserved bits.
	Local0 = ^MCHC.TLUD & (0xfff << 20)
	Local1 = ^MCHC.MEBA

	// Check if ME base is equal
	If (Local0 == Local1) {
		// Use Top Of Memory instead
		// Lower 20 bits of TOM register need to be masked since they contain lock and
		// reserved bits.
		Local0 = ^MCHC.TOM & (0x7ffff << 20)
	}

	PMIN = Local0
	PMAX = CONFIG_ECAM_MMCONF_BASE_ADDRESS - 1
	PLEN = (PMAX - PMIN) + 1

#if CONFIG(SOC_INTEL_BROADWELL)
	// Set up PM02 region (MMCONF end to 0xFEBFFFFF)
	CreateDwordField (MCRS, ^PM02._MIN, PM2B)
	CreateDwordField (MCRS, ^PM02._MAX, PM2M)
	CreateDwordField (MCRS, ^PM02._LEN, PM2L)

	// Calculate ECAM/MMCONF end address based on size bits in PCIEXBAR
	// Bits [2:1] encode size: 00=256MB, 01=128MB, 10=64MB, 11=reserved
	Local2 = ^MCHC.PXSZ

	// Populate based on ECAM size
	If (Local2 == 0) {
		// 256MB ECAM - no space for PM02 (would extend to 4GB boundary)
		PM2L = 0
	} Else {
		// 128MB or 64MB ECAM - there's space for PM02
		Local3 = CONFIG_ECAM_MMCONF_BASE_ADDRESS
		If (Local2 == 1) {
			// 128MB ECAM - ends at base + 0x08000000
			Local3 += 0x08000000
		} Else {
			// 64MB ECAM (or reserved=3) - ends at base + 0x04000000
			Local3 += 0x04000000
		}

		PM2B = Local3
		PM2M = 0xFEBFFFFF  // Just before chipset reserved (IOAPIC at 0xFEC00000)
		PM2L = PM2M - PM2B + 1
	}
#endif
	Return (MCRS)
}

/* PCI Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID ("PNP0C02"))
	Name (_UID, 1)

	Name (PDRS, ResourceTemplate () {
		Memory32Fixed (ReadWrite, CONFIG_FIXED_RCBA_MMIO_BASE, CONFIG_RCBA_LENGTH)
		Memory32Fixed (ReadWrite, CONFIG_FIXED_MCHBAR_MMIO_BASE, MCH_BASE_SIZE)
		Memory32Fixed (ReadWrite, CONFIG_FIXED_DMIBAR_MMIO_BASE, DMI_BASE_SIZE)
		Memory32Fixed (ReadWrite, CONFIG_FIXED_EPBAR_MMIO_BASE, EP_BASE_SIZE)
		Memory32Fixed (ReadWrite, 0xfed20000, 0x00020000) // TXT
		Memory32Fixed (ReadWrite, 0xfed40000, 0x00005000) // TPM
		Memory32Fixed (ReadWrite, 0xfed45000, 0x0004b000) // Misc ICH
		Memory32Fixed (ReadWrite, EDRAM_BASE_ADDRESS, EDRAM_BASE_SIZE)
		Memory32Fixed (ReadWrite, GDXC_BASE_ADDRESS, GDXC_BASE_SIZE)
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		Return (PDRS)
	}
}

/* Configurable TDP */
#include "ctdp.asl"

#if !CONFIG(INTEL_LYNXPOINT_LP)
/* PCI Express Graphics */
#include "peg.asl"
#endif

/* Integrated graphics 0:2.0 */
#include <drivers/intel/gma/acpi/gfx.asl>

/* Intel Mini-HD 0:03.0 */
Device (HDAU)
{
	Name (_ADR, 0x00030000)
}
