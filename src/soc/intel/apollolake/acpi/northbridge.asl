/* SPDX-License-Identifier: GPL-2.0-or-later */


Name(_HID, EISAID("PNP0A08"))	/* PCIe */
Name(_CID, EISAID("PNP0A03"))	/* PCI */

Device (MCHC)
{
	Name (_ADR, 0x00000000)		/*Dev0 Func0 */

	OperationRegion (MCHP, PCI_Config, 0x00, 0x100)
	Field (MCHP, DWordAcc, NoLock, Preserve)
	{
		Offset(0x60),
		MCNF,	32,	/* PCI MMCONF base */
		Offset (0xA8),
		TUUD, 64,	/* Top of Upper Used Memory */
		Offset(0xB4),
		BGSM,   32,	/* Base of Graphics Stolen Memory */
		Offset(0xBC),
		TLUD,   32,	/* Top of Low Usable DRAM */
	}
}

External (A4GS, IntObj)
External (A4GB, IntObj)

/* Current Resource Settings */
Method (_CRS, 0, Serialized)
{
	Name (MCRS, ResourceTemplate()
	{
		/* Bus Numbers */
		WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
				0x0000, 0x0000, 0x00ff, 0x0000, 0x0100,,,)

		/* IO Region 0 */
		DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
				0x0000, 0x0000, 0x0cf7, 0x0000, 0x0cf8,,,)

		/* PCI Config Space */
		Io (Decode16, 0x0cf8, 0x0cf8, 0x0001, 0x0008)

		/* IO Region 1 */
		DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
				0x0000, 0x01000, 0xffff, 0x0000, 0xf000,,,)

		/* VGA memory (0xa0000-0xbffff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000a0000, 0x000bffff, 0x00000000,
				0x00020000,,,)

		/* Data and GFX stolen memory */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x3be00000, 0x3fffffff, 0x00000000,
				0x04200000,,, STOM)

		/*
		 * PCI MMIO Region (TOLUD - PCI extended base MMCONF)
		 * This assumes that MMCONF is placed after PCI config space,
		 * and that no resources are allocated after the MMCONF region.
		 * This works, since MMCONF is hardcoded to 0xe00000000.
		 */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				NonCacheable, ReadWrite,
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000,,, PM01)

		/* PCI Memory Region (TOUUD - (TOUUD + ABOVE_4G_MMIO_SIZE)) */
		QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				NonCacheable, ReadWrite,
				0x00000000, 0x10000, 0x1ffff, 0x00000000,
				0x10000,,, PM02)
	})

	/* Find PCI resource area in MCRS */
	CreateDwordField (MCRS, PM01._MIN, PMIN)
	CreateDwordField (MCRS, PM01._MAX, PMAX)
	CreateDwordField (MCRS, PM01._LEN, PLEN)

	/* Read C-Unit PCI CFG Reg. 0xBC for TOLUD (shadow from B-Unit) */
	PMIN = \_SB.PCI0.MCHC.TLUD & 0xFFF00000
	/* Use PCR base to ensure PMAX below GPIO controllers attached to _SB */
	PMAX = CONFIG_PCR_BASE_ADDRESS & 0xF0000000

	/* Calculate PCI MMIO Length */
	PLEN = PMAX - PMIN + 1

	/* Find GFX resource area in GCRS */
	CreateDwordField(MCRS, STOM._MIN, GMIN)
	CreateDwordField(MCRS, STOM._MAX, GMAX)
	CreateDwordField(MCRS, STOM._LEN, GLEN)

	/* Read BGSM */
	GMIN = \_SB.PCI0.MCHC.BGSM & 0xFFF00000

	/* Read TOLUD */
	GMAX = \_SB.PCI0.MCHC.TLUD & 0xFFF00000
	GMAX--
	GLEN = GMAX - GMIN + 1

	/* Patch PM02 range based on Memory Size */
	If (A4GS == 0) {
		CreateQwordField (MCRS, PM02._LEN, MSEN)
		MSEN = 0
	} Else {
		CreateQwordField (MCRS, PM02._MIN, MMIN)
		CreateQwordField (MCRS, PM02._MAX, MMAX)
		CreateQwordField (MCRS, PM02._LEN, MLEN)
		/* Set 64bit MMIO resource base and length */
		MLEN = A4GS
		MMIN = A4GB
		MMAX = MMIN + MLEN - 1
	}

	Return (MCRS)
}

/* GFX 00:02.0 */
#include <drivers/intel/gma/acpi/gfx.asl>
