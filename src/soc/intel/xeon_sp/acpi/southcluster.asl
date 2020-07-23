/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/iomap.h>

Name(_HID,EISAID("PNP0A08"))	// PCIe
Name(_CID,EISAID("PNP0A03"))	// PCI

Name(_BBN, 0)

Name (MCRS, ResourceTemplate() {
	// Bus Numbers
	WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
		0x0000, 0x0000, 0x00fe, 0x0000, 0xff,,, PB00)

	// IO Region 0
	DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000, 0x0000, 0x0cf7, 0x0000, 0x0cf8,,, PI00)

	// PCI Config Space
	Io (Decode16, 0x0cf8, 0x0cf8, 0x0001, 0x0008)

	// IO Region 1
	DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000, 0x0d00, 0xefff, 0x0000, 0xE300,,, PI01)

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
		0x00000000, 0x90000000, 0xFEAFFFFF, 0x00000000,
		0x6EB00000,,, PMEM)

	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
		Cacheable, ReadWrite,
		0x00000000, 0xfec00000, 0xfecfffff, 0x00000000,
		0x00100000,,, APIC)

	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
		Cacheable, ReadWrite,
		0x00000000, 0xfed00000, 0xfedfffff, 0x00000000,
		0x00100000,,, PCHR)

	QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
		0x0000000000000000, // Granularity
		0x0000380000000000, // Range Minimum
		0x0000383FFFFFFFFF, // Range Maximum
		0x0000000000000000, // Translation Offset
		0x0000004000000000, // Length
		,,, AddressRangeMemory, TypeStatic)
})

Method (_CRS, 0, Serialized) {
	Return (MCRS)
}

Method (_OSC, 4) {
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


Name (AR00, Package() {
	// [DMI0]: Legacy PCI Express Port 0 on PCI0
	Package() { 0x0000FFFF, 0, 0, 47 },
	// [BR1A]: PCI Express Port 1A on PCI0
	// [BR1B]: PCI Express Port 1B on PCI0
	Package() { 0x0001FFFF, 0, 0, 47 },
	// [BR2A]: PCI Express Port 2A on PCI0
	// [BR2B]: PCI Express Port 2B on PCI0
	// [BR2C]: PCI Express Port 2C on PCI0
	// [BR2D]: PCI Express Port 2D on PCI0
	Package() { 0x0002FFFF, 0, 0, 47 },
	// [BR3A]: PCI Express Port 3A on PCI0
	// [BR3B]: PCI Express Port 3B on PCI0
	// [BR3C]: PCI Express Port 3C on PCI0
	// [BR3D]: PCI Express Port 3D on PCI0
	Package() { 0x0003FFFF, 0, 0, 47 },
	// [CB0A]: CB3DMA on PCI0
	// [CB0E]: CB3DMA on PCI0
	Package() { 0x0004FFFF, 0, 0, 31 },
	// [CB0B]: CB3DMA on PCI0
	// [CB0F]: CB3DMA on PCI0
	Package() { 0x0004FFFF, 1, 0, 39 },
	// [CB0C]: CB3DMA on PCI0
	// [CB0G]: CB3DMA on PCI0
	Package() { 0x0004FFFF, 2, 0, 31 },
	// [CB0D]: CB3DMA on PCI0
	// [CB0H]: CB3DMA on PCI0
	Package() { 0x0004FFFF, 3, 0, 39 },
	// [IIM0]: IIOMISC on PCI0
	Package() { 0x0005FFFF, 0, 0, 16 },
	Package() { 0x0005FFFF, 1, 0, 17 },
	Package() { 0x0005FFFF, 2, 0, 18 },
	Package() { 0x0005FFFF, 3, 0, 19 },
	// [IID0]: IIODFX0 on PCI0
	Package() { 0x0006FFFF, 0, 0, 16 },
	Package() { 0x0006FFFF, 1, 0, 17 },
	Package() { 0x0006FFFF, 2, 0, 18 },
	Package() { 0x0006FFFF, 3, 0, 19 },
	// [XHCI]: xHCI controller 1 on PCH
	Package() { 0x0014FFFF, 3, 0, 19 },
	// [HECI]: ME HECI on PCH
	// [IDER]: ME IDE redirect on PCH
	Package() { 0x0016FFFF, 0, 0, 16 },
	// [HEC2]: ME HECI2 on PCH
	// [MEKT]: MEKT on PCH
	Package() { 0x0016FFFF, 1, 0, 17 },
	// [GBEM]: GbE Controller VPRO
	Package() { 0x0019FFFF, 0, 0, 20 },
	// [EHC2]: EHCI controller #2 on PCH
	Package() { 0x001AFFFF, 2, 0, 18 },
	// [ALZA]: High definition Audio Controller
	Package() { 0x001BFFFF, 0, 0, 22 },
	// [RP01]: Pci Express Port 1 on PCH
	// [RP05]: Pci Express Port 5 on PCH
	Package() { 0x001CFFFF, 0, 0, 16 },
	// [RP02]: Pci Express Port 2 on PCH
	// [RP06]: Pci Express Port 6 on PCH
	Package() { 0x001CFFFF, 1, 0, 17 },
	// [RP03]: Pci Express Port 3 on PCH
	// [RP07]: Pci Express Port 7 on PCH
	Package() { 0x001CFFFF, 2, 0, 18 },
	// [RP04]: Pci Express Port 4 on PCH
	// [RP08]: Pci Express Port 8 on ICH
	Package() { 0x001CFFFF, 3, 0, 19 },
	// [EHC1]: EHCI controller #1 on PCH
	Package() { 0x001DFFFF, 2, 0, 18 },
	// [SAT1]: SATA controller 1 on PCH
	// [SAT2]: SATA Host controller 2 on PCH
	Package() { 0x001FFFFF, 0, 0, 16 },
	// [SMBS]: SMBus controller on PCH
	// [TERM]: Thermal Subsystem on ICH
	Package() { 0x001FFFFF, 2, 0, 18 },
	Package() { 0x0017FFFF, 0, 0, 20 },
	Package() { 0x0011FFFF, 0, 0, 21 },
})

// Socket 0 Root bridge
Method (_PRT, 0) {
	Return (AR00)
}
