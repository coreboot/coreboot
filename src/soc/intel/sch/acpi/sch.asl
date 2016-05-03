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

#include "hostbridge.asl"

/* PCI Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 1)

	// This does not seem to work correctly yet - set values statically for
	// now.

	//Name (PDRS, ResourceTemplate() {
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00004000, RCRB) // RCBA
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00004000, MCHB) // MCHBAR
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00001000, DMIB) // DMIBAR
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00001000, EGPB) // EPBAR
	//	Memory32Fixed(ReadWrite, 0x00000000, 0x00000000, PCIE) // PCIE BAR
	//	Memory32Fixed(ReadWrite, 0xfed20000, 0x00070000, ICHB) // Misc ICH
	//})

	Name (PDRS, ResourceTemplate() {
		Memory32Fixed(ReadWrite, 0xfed1c000, 0x00004000) // RCBA
		Memory32Fixed(ReadWrite, 0xfed14000, 0x00004000) // MCHBAR
		Memory32Fixed(ReadWrite, 0xfed18000, 0x00001000) // DMIBAR
		Memory32Fixed(ReadWrite, 0xfed19000, 0x00001000) // EPBAR
		Memory32Fixed(ReadWrite, 0xf0000000, 0x04000000) // PCIE BAR
		Memory32Fixed(ReadWrite, 0xfed20000, 0x00020000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed40000, 0x00005000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed45000, 0x0004b000) // Misc ICH
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		//CreateDwordField(PDRS, ^RCRB._BAS, RBR0)
		//ShiftLeft(\_SB.PCI0.LPCB.RCBA, 14, RBR0)

		//CreateDwordField(PDRS, ^MCHB._BAS, MBR0)
		//ShiftLeft(\_SB.PCI0.MCHC.MHBR, 14, MBR0)

		//CreateDwordField(PDRS, ^DMIB._BAS, DBR0)
		//ShiftLeft(\_SB.PCI0.MCHC.DMBR, 12, DBR0)

		//CreateDwordField(PDRS, ^EGPB._BAS, EBR0)
		//ShiftLeft(\_SB.PCI0.MCHC.EPBR, 12, EBR0)

		//CreateDwordField(PDRS, ^PCIE._BAS, PBR0)
		//ShiftLeft(\_SB.PCI0.MCHC.PXBR, 26, PBR0)

		//CreateDwordField(PDRS, ^PCIE._LEN, PSZ0)
		//ShiftLeft(0x10000000, \_SB.PCI0.MCHC.PXSZ, PSZ0)

		Return(PDRS)
	}
}

// PCIe graphics port 0:1.0
#include "peg.asl"

// Integrated graphics 0:2.0
#include "igd.asl"

/* Intel PCH support */

Scope(\)
{
	// IO-Trap at 0x800. This is the ACPI->SMI communication interface.

	OperationRegion(IO_T, SystemIO, 0x800, 0x10)
	Field(IO_T, ByteAcc, NoLock, Preserve)
	{
		Offset(0x8),
		TRP0, 8		// IO-Trap at 0x808
	}

	// ICH7 Power Management Registers, located at PMBASE (0x1f.0 0x40.l)
	// this doesn't work as ACPI initializes regions and packages first, devices second.
	// use dynamic operation region? if so, how? XXX
        //OperationRegion(PMIO, SystemIO, And(\_SB_.PCI0.LPCB.PMBS, 0xfffc), 0x80)
	OperationRegion(PMIO, SystemIO, 0x500, 0x80)
	Field(PMIO, ByteAcc, NoLock, Preserve)
	{
		Offset(0x42),	// General Purpose Control
		, 1,		// skip 1 bit
		GPEC, 1,	// TCO status
		, 9,		// skip 9 more bits
		SCIS, 1,	// TCO DMI status
		, 6		// To the end of the word
	}

	// ICH7 GPIO IO mapped registers (0x1f.0 reg 0x48.l)
	OperationRegion(GPIO, SystemIO, 0x1180, 0x3c)
	Field(GPIO, ByteAcc, NoLock, Preserve)
	{
		Offset(0x00),	// GPIO Use Select
		GU00, 8,
		GU01, 8,
		GU02, 8,
		GU03, 8,
		Offset(0x04),	// GPIO IO Select
		GIO0, 8,
		GIO1, 8,
		GIO2, 8,
		GIO3, 8,
		Offset(0x0c),	// GPIO Level
		GL00, 8,
		GL01, 8,
		, 3,
		GP27, 1,	// SATA_PWR_EN #0
		GP28, 1,	// SATA_PWR_EN #1
		, 3,
		Offset(0x18),	// GPIO Blink
		GB00, 8,
		GB01, 8,
		GB02, 8,
		GB03, 8,
		Offset(0x2c),	// GPIO Invert
		GIV0, 8,
		GIV1, 8,
		GIV2, 8,
		GIV3, 8,
		Offset(0x30),	// GPIO Use Select 2
		GU04, 8,
		GU05, 8,
		GU06, 8,
		GU07, 8,
		Offset(0x34),	// GPIO IO Select 2
		GIO4, 8,
		GIO5, 8,
		GIO6, 8,
		GIO7, 8,
		Offset(0x38),	// GPIO Level 2
		, 5,
		GP37, 1,	// PATA_PWR_EN
		GP38, 1,	// Battery / Power (?)
		GP39, 1,	// ??
		GL05, 8,
		GL06, 8,
		GL07, 8
	}


	// ICH7 Root Complex Register Block. Memory Mapped through RCBA)
	OperationRegion(RCRB, SystemMemory, 0xfed1c000, 0x4000)
	Field(RCRB, DWordAcc, Lock, Preserve)
	{
		Offset(0x0000), // Backbone
		Offset(0x1000), // Chipset
		Offset(0x3000), // Legacy Configuration Registers
		Offset(0x3404), // High Performance Timer Configuration
		HPAS, 2, 	// Address Select
		, 5,
		HPTE, 1,	// Address Enable
		Offset(0x3418), // FD (Function Disable)
		, 1,		// Reserved
		PATD, 1,	// PATA disable
		SATD, 1,	// SATA disable
		SMBD, 1,	// SMBUS disable
		HDAD, 1,	// Azalia disable
		A97D, 1,	// AC'97 disable
		M97D, 1,	// AC'97 disable
		ILND, 1,	// Internal LAN disable
		US1D, 1,	// UHCI #1 disable
		US2D, 1,	// UHCI #2 disable
		US3D, 1,	// UHCI #3 disable
		US4D, 1,	// UHCI #4 disable
		, 2,		// Reserved
		LPBD, 1,	// LPC bridge disable
		EHCD, 1,	// EHCI disable
		Offset(0x341a), // FD Root Ports
		RP1D, 1,	// Root Port 1 disable
		RP2D, 1,	// Root Port 2 disable
		RP3D, 1,	// Root Port 3 disable
		RP4D, 1,	// Root Port 4 disable
		RP5D, 1,	// Root Port 5 disable
		RP6D, 1		// Root Port 6 disable
	}

}

// 0:1b.0 High Definition Audio (Azalia)
#include "audio.asl"

// PCI Express Ports
#include "pcie.asl"

// USB
#include "usb.asl"

// PCI Bridge
#include "pci.asl"

// AC97 Audio and Modem
#include "ac97.asl"

// LPC Bridge
#include "lpc.asl"

// PATA
#include "pata.asl"

// SMBus
#include "smbus.asl"
