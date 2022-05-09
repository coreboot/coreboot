/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <soc/iomap.h>

Name (_HID, EisaId ("PNP0A08") /* PCI Express Bus */)  // _HID: Hardware ID
Name (_CID, EisaId ("PNP0A03") /* PCI Bus */)  // _CID: Compatible ID
Name (_SEG, Zero)  // _SEG: PCI Segment
Name (_UID, Zero)  // _UID: Unique ID

Device (MCHC)
{
	Name (_ADR, 0x00000000)

	OperationRegion (MCHP, PCI_Config, 0x00, 0x100)
	Field (MCHP, DWordAcc, NoLock, Preserve)
	{
		Offset(0x40),	/* EPBAR (0:0:0:40) */
		EPEN, 1,	/* Enable */
		, 11,
		EPBR, 27,	/* EPBAR [38:12] */

		Offset(0x48),	/* MCHBAR (0:0:0:48) */
		MHEN, 1,	/* Enable */
		, 14,
		MHBR, 24,	/* MCHBAR [38:15] */

		Offset(0x60),	/* PCIEXBAR (0:0:0:60) */
		PXEN, 1,	/* Enable */
		PXSZ, 2,	/* PCI Express Size */
		, 23,
		PXBR, 13,	/* PCI Express BAR [38:26] */

		Offset(0x68),	/* DMIBAR (0:0:0:68) */
		DIEN, 1,	/* Enable */
		, 11,
		DIBR, 27,	/* DMIBAR [38:12] */

		Offset (0x70),	/* ME Base Address */
		MEBA, 64,

		Offset (0xa0),
		TOM, 64,	/* Top of Used Memory */
		TUUD, 64,	/* Top of Upper Used Memory */

		Offset (0xbc),	/* Top of Low Used Memory */
		TLUD, 32,
	}
}

External (A4GS, IntObj)
External (A4GB, IntObj)

Method (_CRS, 0, Serialized)
{
	Name (MCRS, ResourceTemplate ()
	{
		/* Bus Numbers */
		WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
				0x0000, 0x0000, 0x00ff, 0x0000, 0x0100)

		/* IO Region 0 */
		DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode,
				EntireRange,
				0x0000, 0x0000, 0x0cf7, 0x0000, 0x0cf8)

		/* PCI Config Space */
		Io (Decode16, 0x0cf8, 0x0cf8, 0x0001, 0x0008)

		/* IO Region 1 */
		DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode,
				EntireRange,
				0x0000, 0x0d00, 0xffff, 0x0000, 0xf300)

		/* VGA memory (0xa0000-0xbffff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000a0000, 0x000bffff, 0x00000000,
				0x00020000)

		/*  OPROM reserved (0xc0000-0xc3fff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000c0000, 0x000c3fff, 0x00000000,
				0x00004000)

		/* OPROM reserved (0xc4000-0xc7fff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000c4000, 0x000c7fff, 0x00000000,
				0x00004000)

		/* OPROM reserved (0xc8000-0xcbfff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000c8000, 0x000cbfff, 0x00000000,
				0x00004000)

		/* OPROM reserved (0xcc000-0xcffff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000cc000, 0x000cffff, 0x00000000,
				0x00004000)

		/* OPROM reserved (0xd0000-0xd3fff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000d0000, 0x000d3fff, 0x00000000,
				0x00004000)

		/* OPROM reserved (0xd4000-0xd7fff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000d4000, 0x000d7fff, 0x00000000,
				0x00004000)

		/* OPROM reserved (0xd8000-0xdbfff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000d8000, 0x000dbfff, 0x00000000,
				0x00004000)

		/* OPROM reserved (0xdc000-0xdffff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000dc000, 0x000dffff, 0x00000000,
				0x00004000)

		/* BIOS Extension (0xe0000-0xe3fff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000e0000, 0x000e3fff, 0x00000000,
				0x00004000)

		/* BIOS Extension (0xe4000-0xe7fff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000e4000, 0x000e7fff, 0x00000000,
				0x00004000)

		/* BIOS Extension (0xe8000-0xebfff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000e8000, 0x000ebfff, 0x00000000,
				0x00004000)

		/* BIOS Extension (0xec000-0xeffff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000ec000, 0x000effff, 0x00000000,
				0x00004000)

		/* System BIOS (0xf0000-0xfffff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000f0000, 0x000fffff, 0x00000000,
				0x00010000)

		/* PCI Memory Region (TLUD - 0xdfffffff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				NonCacheable, ReadWrite,
				0x00000000, 0x00000000, 0xdfffffff, 0x00000000,
				0xE0000000,,, PM01)

		/* PCI Memory Region (TUUD - (TUUD + ABOVE_4G_MMIO_SIZE)) */
		QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				NonCacheable, ReadWrite,
				0x00000000, 0x10000, 0x1ffff, 0x00000000,
				0x10000,,, PM02)

		/* PCH reserved resource (0xfc800000-0xfe7fffff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, PCH_PRESERVED_BASE_ADDRESS, 0xfe7fffff,
				0x00000000, PCH_PRESERVED_BASE_SIZE)

#if !CONFIG(TPM_GOOGLE)
		/* TPM Area (0xfed40000-0xfed44fff) */
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0xfed40000, 0xfed44fff, 0x00000000,
				0x00005000)
#endif
})

	/* Find PCI resource area in MCRS */
	CreateDwordField (MCRS, PM01._MIN, PMIN)
	CreateDwordField (MCRS, PM01._MAX, PMAX)
	CreateDwordField (MCRS, PM01._LEN, PLEN)

	/*
	 * Fix up PCI memory region
	 * Start with Top of Lower Usable DRAM
	 * Lower 20 bits of TOLUD register need to be masked since they contain lock and
	 * reserved bits.
	 */
	Local0 = \_SB.PCI0.MCHC.TLUD & (0xfff << 20)
	Local1 = \_SB.PCI0.MCHC.MEBA

	/*  Check if ME base is equal */
	If (Local0 == Local1) {
		/*
		 * Use Top Of Memory instead
		 * Lower 20 bits of TOM register need to be masked since they contain lock and
		 * reserved bits.
		 */
		Local0 = \_SB.PCI0.MCHC.TOM & (0x7ffff << 20)
	}

	PMIN = Local0
	PLEN = (PMAX - PMIN) + 1

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
		MMAX = (MMIN + MLEN) - 1
	}

	Return (MCRS)
}

/* Get MCH BAR */
Method (GMHB, 0, Serialized)
{
	Local0 = \_SB.PCI0.MCHC.MHBR << 15
	Return (Local0)
}

/* Get EP BAR */
Method (GEPB, 0, Serialized)
{
	Local0 = \_SB.PCI0.MCHC.EPBR << 12
	Return (Local0)
}

/* Get PCIe BAR */
Method (GPCB, 0, Serialized)
{
	Local0 = \_SB.PCI0.MCHC.PXBR << 26
	Return (Local0)
}

/* Get PCIe Length */
Method (GPCL, 0, Serialized)
{
	Local0 = 0x10000000 >> \_SB.PCI0.MCHC.PXSZ
	Return (Local0)
}

/* Get DMI BAR */
Method (GDMB, 0, Serialized)
{
	Local0 = \_SB.PCI0.MCHC.DIBR << 12
	Return (Local0)
}

/* PCI Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID ("PNP0C02"))
	Name (_UID, 1)

	Method (_CRS, 0, Serialized)
	{
		Name (BUF0, ResourceTemplate ()
		{
			/* MCH BAR _BAS will be updated in _CRS below according to
			 * B0:D0:F0:Reg.48h
			 */
			Memory32Fixed (ReadWrite, 0, 0x08000, MCHB)

			/* DMI BAR _BAS will be updated in _CRS below according to
			 * B0:D0:F0:Reg.68h
			 */
			Memory32Fixed (ReadWrite, 0, 0x01000, DMIB)

			/* EP BAR _BAS will be updated in _CRS below according to
			 * B0:D0:F0:Reg.40h
			 */
			Memory32Fixed (ReadWrite, 0, 0x01000, EGPB)

			/* PCI Express BAR _BAS and _LEN will be updated in
			 * _CRS below according to B0:D0:F0:Reg.60h
			 */
			Memory32Fixed (ReadWrite, 0, 0, PCIX)

			/* VTD engine memory range. */
			Memory32Fixed (ReadOnly, VTD_BASE_ADDRESS, VTD_BASE_SIZE)

			/* FLASH range */
			Memory32Fixed (ReadOnly, 0, CONFIG_ROM_SIZE, FIOH)

			/* Local APIC range(0xFEE0_0000 to 0xFEEF_FFFF) */
			Memory32Fixed (ReadOnly, 0xFEE00000, 0x100000)

			/* HPET address decode range */
			Memory32Fixed (ReadWrite, HPET_BASE_ADDRESS, 0x400)
		})

		CreateDwordField (BUF0, MCHB._BAS, MBR0)
		MBR0 = \_SB.PCI0.GMHB ()

		CreateDwordField (BUF0, DMIB._BAS, DBR0)
		DBR0 = \_SB.PCI0.GDMB ()

		CreateDwordField (BUF0, EGPB._BAS, EBR0)
		EBR0 = \_SB.PCI0.GEPB ()

		CreateDwordField (BUF0, PCIX._BAS, XBR0)
		XBR0 = \_SB.PCI0.GPCB ()

		CreateDwordField (BUF0, PCIX._LEN, XSZ0)
		XSZ0 = \_SB.PCI0.GPCL ()

		CreateDwordField (BUF0, FIOH._BAS, FBR0)
		FBR0 = 0x100000000 - CONFIG_ROM_SIZE

		Return (BUF0)
	}
}

/* Integrated graphics 0:2.0 */
#include <drivers/intel/gma/acpi/gfx.asl>
