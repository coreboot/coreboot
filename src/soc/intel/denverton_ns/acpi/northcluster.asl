/* SPDX-License-Identifier: GPL-2.0-only */

#include "../include/soc/iomap.h"

Name(_HID,EISAID("PNP0A08"))	// PCIe
Name(_CID,EISAID("PNP0A03"))	// PCI

Name(_BBN, 0)

Device (MCHC)
{
	Name(_ADR, 0x00000000)	// 0:0.0

	OperationRegion(MCHP, PCI_Config, 0x00, 0x100)
	Field (MCHP, DWordAcc, NoLock, Preserve)
	{
		Offset (0x48),	// MCHBAR
		MHEN,	 1,	// Enable
		,	13,	//
		MHBR,	22,	// MCHBAR

		Offset (0x60),	// PCIe BAR
		PXEN,	 1,	// Enable
		PXSZ,	 2,	// BAR size
		,	23,	//
		PXBR,	10,	// PCIe BAR

		Offset (0xa8),	// Top of Upper Memory
		TUUD,	 64,

		Offset (0xb8),	// TSEGMB
		TSEG,	 32,

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

	// RAM (0xc0000-0xdffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000c0000, 0x000dffff, 0x00000000,
			0x00020000,,, OPR0)

	// PCI Memory Region (Top of memory-PCIEXBAR)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			NonCacheable, ReadWrite,
			0x00000000, 0x00000000, 0xfebfffff, 0x00000000,
			0xfec00000,,, PM01)

#ifdef ENABLE_TPM
	// TPM Area (0xfed40000-0xfed44fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0xfed40000, 0xfed44fff, 0x00000000,
			0x00005000,,, TPMR)
#endif

	// PCI Memory Region (TOUUD - 64G)
	QWORDMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			NonCacheable, ReadWrite,
			0x00000000, 0x00000000, 0x1ffffffff, 0x00000000,
			0x200000000,,, PM02)

})	// End MCRS

Method (_CRS, 0, Serialized)
{
	// Find PCI resource area in MCRS
	CreateDwordField(MCRS, ^PM01._MIN, PMIN)
	CreateDwordField(MCRS, ^PM01._MAX, PMAX)
	CreateDwordField(MCRS, ^PM01._LEN, PLEN)

	// MMIO Low is saved in NVS
	PMIN = \MMOB
	PMAX = \MMOL
	PLEN = PMAX - PMIN + 1

	// Find PCI resource area in MCRS
	CreateQWordField(MCRS, ^PM02._MIN, P2MN)
	CreateQWordField(MCRS, ^PM02._MAX, P2MX)
	CreateQWordField(MCRS, ^PM02._LEN, P2LN)

	// MMIO High is saved in NVS
	P2MN = \MMHB
	P2MX = \MMHL
	P2LN = P2MX - P2MN +1

	Return (MCRS)
}	// End _CRS

/* PCI Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 1)

	Name (PDRS, ResourceTemplate() {
		// PCIEXBAR memory range
		Memory32Fixed(ReadOnly, CONFIG_MMCONF_BASE_ADDRESS, 0x10000000)
		// TSEG
		Memory32Fixed(ReadOnly, 0x00000000, 0x00000000, TSMB)
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		// Fix up 32-bit TSEG
		CreateDWordField(PDRS, ^TSMB._BAS, TSMN)
		TSMN = \TSGB
		CreateDWordField(PDRS, ^TSMB._LEN, TSLN)
		TSLN = \TSSZ
		Return(PDRS)
	}
}

// Global Registers
Device (GREG) {
	Name   (_ADR, 0x00040000)
}

// Root Complex Event Collector
Device (RCEC) {
	Name   (_ADR, 0x00050000)
}

// Virtual root port 2
Device (VRP2) {
	Name   (_ADR, 0x00060000)

	Method (_PRT)
	{
		Return (IRQM (6))
	}
}
