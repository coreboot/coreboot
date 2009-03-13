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


Name(_HID,EISAID("PNP0A08"))	// PCIe
Name(_CID,EISAID("PNP0A03"))	// PCI

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

		Offset (0x48),	// MCHBAR
		MHEN,	 1,	// Enable
		,	13,	//
		MHBR,	18,	// MCHBAR

		Offset (0x60),	// PCIe BAR
		PXEN,	 1,	// Enable
		PXSZ,	 2,	// BAR size
		,	23,	//
		PXBR,	 6,	// PCIe BAR

		Offset (0x68),	// DMIBAR
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
		Offset (0xa2),	// Top of upper used dram
		TUUD,	16,
		Offset (0xb0),
		,	 4,
		TLUD,	12	// TOLUD
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

		// Bios Extension (0xe0000-0xeffff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000e0000, 0x000effff, 0x00000000,
				0x00010000,,, ESEG)

		// System BIOS (0xf0000-0xfffff)
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				Cacheable, ReadWrite,
				0x00000000, 0x000f0000, 0x000fffff, 0x00000000,
				0x00010000,,, FSEG)

		 // PCI Memory Region (Top of memory-0xfebfffff)
		 DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
				 Cacheable, ReadWrite,
				 0x00000000, 0x00000000, 0xfebfffff, 0x00000000,
				 0x00000000,,, PM01)
	})

	// Find PCI resource area in MCRS
	CreateDwordField(MCRS, PM01._MIN, PMIN)
	CreateDwordField(MCRS, PM01._MAX, PMAX)
	CreateDwordField(MCRS, PM01._LEN, PLEN)

	// Fix up PCI memory region:
	// Enter actual TOLUD. The TOLUD register contains bits 20-31 of
	// the top of memory address.
	ShiftLeft (^MCHC.TLUD, 20, PMIN)
	Add(Subtract(PMAX, PMIN), 1, PLEN)

	Return (MCRS)
}

// PCI Interrupt Routing
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			// PCIe Graphics		0:1.0
			Package() { 0x0001ffff, 0, 0, 16 },
			// Onboard graphics (IGD)	0:2.0
			Package() { 0x0002ffff, 0, 0, 16 },
			// High Definition Audio	0:1b.0
			Package() { 0x001bffff, 0, 0, 16 },
			// PCIe Root Ports		0:1c.x
			Package() { 0x001cffff, 0, 0, 16 },
			Package() { 0x001cffff, 1, 0, 17 },
			Package() { 0x001cffff, 2, 0, 18 },
			Package() { 0x001cffff, 3, 0, 19 },
			// USB and EHCI			0:1d.x
			Package() { 0x001dffff, 0, 0, 23 },
			Package() { 0x001dffff, 1, 0, 19 },
			Package() { 0x001dffff, 2, 0, 18 },
			Package() { 0x001dffff, 3, 0, 16 },
			// LPC device			0:1f.0
			Package() { 0x001fffff, 0, 0, 19 },
			Package() { 0x001fffff, 1, 0, 19},
			Package() { 0x001fffff, 2, 0, 19 },
			Package() { 0x001fffff, 3, 0, 16 }
		})
	} Else {
		Return (Package() {
			// PCIe Graphics		0:1.0
			Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			// Onboard graphics (IGD)	0:2.0
			Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKA, 0 },
			// High Definition Audio	0:1b.0
			Package() { 0x001bffff, 0, \_SB.PCI0.LPCB.LNKG, 0 },
			// PCIe Root Ports		0:1c.x
			Package() { 0x001cffff, 0, \_SB.PCI0.LPCB.LNKB, 0 },
			Package() { 0x001cffff, 1, \_SB.PCI0.LPCB.LNKA, 0 },
			Package() { 0x001cffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			Package() { 0x001cffff, 3, \_SB.PCI0.LPCB.LNKD, 0 },
			// USB and EHCI			0:1d.x
			Package() { 0x001dffff, 0, \_SB.PCI0.LPCB.LNKH, 0 },
			Package() { 0x001dffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001dffff, 2, \_SB.PCI0.LPCB.LNKC, 0 },
			// LPC device			0:1f.0
			Package() { 0x001fffff, 0, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001fffff, 1, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001fffff, 2, \_SB.PCI0.LPCB.LNKD, 0 },
			Package() { 0x001fffff, 3, \_SB.PCI0.LPCB.LNKA, 0 }
		})
	}

}
