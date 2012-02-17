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

/* Intel 82801Gx support */

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
	OperationRegion(PMIO, SystemIO, DEFAULT_PMBASE, 0x80)
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
	OperationRegion(GPIO, SystemIO, DEFAULT_GPIOBASE, 0x3c)
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
		GP00, 1,
		GP01, 1,
		GP02, 1,
		GP03, 1,
		GP04, 1,
		GP05, 1,
		GP06, 1,	// GDET
		GP07, 1,
		GP08, 1,
		GP09, 1,	// HPMU
		GP10, 1,	// GPSE
		GP11, 1,
		GP12, 1,	// WLED
		GP13, 1,	// BLED
		GP14, 1,	// GLED
		GP15, 1,	// GDIS
		GP16, 1,
		GP17, 1,
		GP18, 1,	// SPCI
		GP19, 1,	// TSDT
		GP20, 1,	// SCPU
		GP21, 1,
		GP22, 1,
		GP23, 1,	// LANP
		GP24, 1,	// DKLR
		GP25, 1,	// WLAN
		GP26, 1,	// SATA_PWR_EN #0 / SPOF
		GP27, 1,	// SATA_PWR_EN #1 / SPMU
		GP28, 1,
		GP29, 1,
		GP30, 1,
		GP31, 1,
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
		GP32, 1,
		GP33, 1,	// CREN
		GP34, 1,	// CRRS
		GP35, 1,
		GP36, 1,	// STAD
		GP37, 1,	// PATA_PWR_EN / HDDE
		GP38, 1,	// Battery / Power (?) / MB00
		GP39, 1,	// ?? / MB01
		GL05, 8,
		GL06, 8,
		GL07, 8
	}


	// ICH7 Root Complex Register Block. Memory Mapped through RCBA)
	OperationRegion(RCRB, SystemMemory, DEFAULT_RCBA, 0x4000)
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

// SATA
#include "sata.asl"

// SMBus
#include "smbus.asl"


