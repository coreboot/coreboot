/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Nick Barker <Nick.Barker9@btinternet.com>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2011,2014 Oskar Enoksson <enok@lysator.liu.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * ISA portions taken from QEMU acpi-dsdt.dsl.
 */

DefinitionBlock ("DSDT.aml", "DSDT", 1, "LXBIOS", "LXB-DSDT", 1)
{
	// Name (SPIO, 0x2E)	// SuperIO (w83627hf)
	Name (SPI2, 0x4E)	// Unknown National Semiconductors (EPM3128A?)
	Name (IO1B, 0x0680)	// GPIO Base (?)
	Name (IO1L, 0x80)
	//Name (IO2B, 0x0295)	// Hardware monitor
	//Name (IO2L, 0x02)
	Name (PMBS, 0x2000)	// Power Management Base
	Name (PMLN, 0xC0)	// Power Management Length
	Name (GPBS, 0x20C0)
	Name (GPLN, 0x20)
	Name (SMBS, 0x20E0)
	Name (SMBL, 0x20)

#define NO_W83627HF_FDC		// don't expose the floppy disk controller
#define NO_W83627HF_FDC_ENUM	// don't try to enumerate the connected floppy drives
#define NO_W83627HF_PPORT	// don't expose the parallel port
//#define NO_W83627HF_UARTA	// don't expose the first serial port
#define NO_W83627HF_UARTB	// don't expose the second serial port (already hidden
				// if UARTB is configured as IRDA port by firmware)
#define NO_W83627HF_IRDA	// don't expose the IRDA port (already hidden if UARTB is
				// configured as serial port by firmware)
#define NO_W83627HF_CIR		// don't expose the Consumer Infrared functionality
//#define NO_W83627HF_KBC	// don't expose the keyboard controller
//#define NO_W83627HF_PS2M	// don't expose the PS/2 mouse functionality of the
				// keyboard controller
#define NO_W83627HF_GAME	// don't expose the game port
#define NO_W83627HF_MIDI	// don't expose the MIDI port
// #define NO_W83627HF_HWMON	// don't expose the hardware monitor as
				// PnP "Motherboard Resource"
// Scope (\_PR) and relevant CPU? objects are auto-generated in SSDT

	Scope (\_SB) {		// Root of the bus hierarchy
		Device (PCI0)	{	// Top PCI device (AMD K8 Northbridge 1)

			Device(MBRS) {
				Name (_HID, EisaId ("PNP0C02"))
				Name (_UID, 0x01)
				External(_CRS) /* Resource Template in SSDT */
			}

			// The following symbols are assumed to be created by coreboot
			External (BUSN)
			External (PCIO)
			External (MMIO)
			External (SBLK)
			External (CBST)
			External (SBDN)
			External (TOM1)  // Top Of Memory 1 (low 4GB ?)
			External (HCLK)  // Hypertransport possible CLocK frequencies
			External (HCDN)  // Hypertransport Controller Device Numbers

			Name (_HID, EisaId ("PNP0A03"))
			Name (_ADR, 0x00180000)
			//Name (_UID, 0x00)
			Name (_UID, 0x01)

			Name (HCIN, 0x00)  // HC1
			Method (_BBN, 0, NotSerialized) {
				Return (GBUS (GHCN(HCIN), GHCL(HCIN)))
			}
			Method (_CRS, 0, Serialized) {
				Name (BUF0, ResourceTemplate () {
				// PCI Configuration address space address/data
					IO (Decode16, 0x0CF8, 0x0CF8, 0x01, 0x08)
					IO (Decode16, 0xC000, 0xC000, 0x01, 0x80) //8000h
					IO (Decode16, 0xC080, 0xC080, 0x01, 0x80) //8080h
					WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
						0x0000, // Address Space Granularity
						0x8100, // Address Range Minimum
						0xFFFF, // Address Range Maximum
						0x0000, // Address Translation Offset
						0x7F00,,,
						, TypeStatic)    //8100h-FFFFh
					DWordMemory (ResourceProducer, PosDecode,
						MinFixed, MaxFixed, Cacheable, ReadWrite,
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
						, TypeStatic)  //0-CF7h
					WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
						0x0000, // Address Space Granularity
						0x03E0, // Address Range Minimum
						0x0CF7, // Address Range Maximum
						0x0000, // Address Translation Offset
						0x0918,,,
						, TypeStatic)  //0-CF7h
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
		Device (PCI1) {
			Name (_HID, "PNP0A03")
			Name (_ADR, 0x00190000)
			Name (_UID, 0x02)
			Method (_STA, 0, NotSerialized) {
				Return (\_SB.PCI0.CBST)
			}
			//Name (HCIN, 0x01)  // HC2
			//Method (_BBN, 0, NotSerialized) {
			//	Return (GBUS (GHCN(HCIN), GHCL(HCIN)))
			//}
			Name (_BBN, 0x00)
		}
		Device (PWRB) {
			Name (_HID, EisaId ("PNP0C0C"))
			Name (_UID, 0xAA)
			Name (_STA, 0x0B)
		}
	}
	Scope (_GPE) {
		Method (_L08, 0, NotSerialized) {
			Notify (\_SB.PCI0, 0x02) //PME# Wakeup
			Notify (\_SB.PCI0.TP2P.ETHR, 0x02)
			Notify (\_SB.PWRB, 0x02)
		}
		Method (_L0F, 0, NotSerialized) {
			Notify (\_SB.PCI0.TP2P.USB0, 0x02)  //USB Wakeup
			Notify (\_SB.PCI0.TP2P.USB1, 0x02)
			Notify (\_SB.PWRB, 0x02)
		}
		Method (_L22, 0, NotSerialized) { // GPIO18 (LID) - Pogo 0 Bridge B
			Notify (\_SB.PCI0.PG0B, 0x02)
			Notify (\_SB.PWRB, 0x02)
		}
		Method (_L29, 0, NotSerialized) { // GPIO25 (Suspend) - Pogo 0 Bridge A
			Notify (\_SB.PCI0.PG0A, 0x02)
			Notify (\_SB.PWRB, 0x02)
		}
	}
	OperationRegion (KSB0, SystemIO, 0x72, 0x02) // CMOS RAM (?)
	Field (KSB0, ByteAcc, NoLock, Preserve) {
		KSBI,   8, // Index
		KSBD,   8  // Data
	}
/*
	OperationRegion (IHHM, SystemIO, IO2B, IO2L) // Hardware monitor
	Field (IHHM, ByteAcc, NoLock, Preserve) {
		HHMI,   8, // Index
		HHMD,   8  // Data
	}
*/
	// Method (_BFS, 1, NotSerialized) {
	// Control method executed immediately following a wake event.
	// Arg0		 => Value of the sleeping state from which woken (1=S1, 2=S2 ...)
	// Optional
	//}

	Method (_PTS, 1, NotSerialized) {
	// Control method used to Prepare To Sleep.
	// Arg0		 => Value of the sleeping state (1=S1, 2=S2 ...)
		Or (Arg0, 0xF0, Local0)
		Store (Local0, DBG8)
	}

	// Method (_GTS, 1, NotSerialized) {
	// Control method executed just prior to setting the sleep enable (SLP_EN) bit.
	// Arg0		 => Value of the sleeping state (1=S1, 2=S2 ...)
	// Optional
	//}

	#include <southbridge/amd/amd8111/acpi/sleepstates.asl>

	Name (WAKP, Package (0x02) { 0x00, 0x00 })
	// Status
	//  0: 0  Wake was signaled but failed due to lack of power.
	//  1: 1  Wake was signaled but failed due to thermal condition
	//  2:31  Reserved
	// PSS
	//  0:1f  If non-zero, the effective S-state the power supply really entered.

	Method (_WAK, 1, NotSerialized) {
	// System Wake
	// Arg0: The value of the sleeping state from which woken (1=S1, ...)
	// Result: (2 DWORD package)
		ShiftLeft (Arg0, 0x04, DBG8)
		Store (0xFF, KSBI) // Clear 0xFF in CMOS RAM
		Store (0x00, KSBD)
		If (LEqual (Arg0, 0x01)) { // Wake from S1 state
			And (\_SB.PCI0.GSTS, 0x10, Local0)
			And (Local0, \_SB.PCI0.GNBL, Local0)
			If (Local0) {
				Notify (\_SB.PWRB, 0x02)
			}
		}
		Store (\_SB.PCI0.GSTS, Local0)
		Store (Local0, \_SB.PCI0.GSTS)
		Store (\_SB.PCI0.STMC, Local0)
		Store (Local0, \_SB.PCI0.STMC)
		Store (\_SB.PCI0.STC0, Local0)
		Store (Local0, \_SB.PCI0.STC0)
		Store (\_SB.PCI0.STC1, Local0)
		Store (Local0, \_SB.PCI0.STC1)
		Store (\_SB.PCI0.STHW, Local0)
		Store (Local0, \_SB.PCI0.STHW)
		If (LEqual (Arg0, 0x03)) { // Wake from S3 state
			Notify (\_SB.PCI0.TP2P.USB0, 0x01)
		}
		Store (0xC0, \_SB.PCI0.SWSM)
		If (DerefOf (Index (WAKP, 0x00))) {
			Store (0x00, Index (WAKP, 0x01))
		} Else {
			Store (Arg0, Index (WAKP, 0x01))
		}
		Return (WAKP)
	}

	Name (PICF, 0x00) //Flag Variable for PIC vs. I/O APIC Mode
	Method (_PIC, 1, NotSerialized) { //PIC Flag and Interface Method
	// Control method that conveys interrupt model in use to the system
	// firmware. OS reports interrupt model in use.
	// 0 => PIC Mode
	// 1 => APIC Mode
	// 2 => SAPIC Mode
	// 3.. => Reserved
		Store (Arg0, PICF)
	}
	OperationRegion (DEB8, SystemIO, 0x80, 0x01)
	Field (DEB8, ByteAcc, Lock, Preserve) {
		DBG8,   8
	}
	OperationRegion (DEB9, SystemIO, 0x90, 0x01)
	Field (DEB9, ByteAcc, Lock, Preserve) {
		DBG9,   8
	}
	OperationRegion (EXTM, SystemMemory, 0x000FF83C, 0x04)
	Field (EXTM, WordAcc, Lock, Preserve) {
		AMEM,   32
	}
	OperationRegion (VGAM, SystemMemory, 0x000C0002, 0x01)
	Field (VGAM, ByteAcc, Lock, Preserve) {
		VGA1,   8    // Video memory length (in 2k units?)
	}
	OperationRegion (GRAM, SystemMemory, 0x0400, 0x0100)
	Field (GRAM, ByteAcc, Lock, Preserve) {
		Offset (0x10),
		FLG0,   8
	}
	OperationRegion (Z007, SystemIO, 0x21, 0x01)
	Field (Z007, ByteAcc, NoLock, Preserve) {
		Z008,   8
	}
	OperationRegion (Z009, SystemIO, 0xA1, 0x01)
	Field (Z009, ByteAcc, NoLock, Preserve) {
		Z00A,   8
	}
	#include "northbridge/amd/amdk8/util.asl"
}
