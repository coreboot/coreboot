/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel Cougar Point PCH support */
#include <southbridge/intel/bd82x6x/pch.h>

Scope(\)
{
	// PCH Power Management Registers, located at PMBASE (0x1f.0 0x40.l)
	OperationRegion(PMIO, SystemIO, DEFAULT_PMBASE, 0x80)
	Field(PMIO, ByteAcc, NoLock, Preserve)
	{
		Offset(0x20),	// GPE0_STS
		, 16,
		GS00, 1,	// GPIO00 SCI/Wake Status
		GS01, 1,	// GPIO01 SCI/Wake Status
		GS02, 1,	// GPIO02 SCI/Wake Status
		GS03, 1,	// GPIO03 SCI/Wake Status
		GS04, 1,	// GPIO04 SCI/Wake Status
		GS05, 1,	// GPIO05 SCI/Wake Status
		GS06, 1,	// GPIO06 SCI/Wake Status
		GS07, 1,	// GPIO07 SCI/Wake Status
		GS08, 1,	// GPIO08 SCI/Wake Status
		GS09, 1,	// GPIO09 SCI/Wake Status
		GS10, 1,	// GPIO10 SCI/Wake Status
		GS11, 1,	// GPIO11 SCI/Wake Status
		GS12, 1,	// GPIO12 SCI/Wake Status
		GS13, 1,	// GPIO13 SCI/Wake Status
		GS14, 1,	// GPIO14 SCI/Wake Status
		GS15, 1,	// GPIO15 SCI/Wake Status
		Offset(0x28),	// GPE0_EN
		, 16,
		GE00, 1,	// GPIO00 SCI/Wake Enable
		GE01, 1,	// GPIO01 SCI/Wake Enable
		GE02, 1,	// GPIO02 SCI/Wake Enable
		GE03, 1,	// GPIO03 SCI/Wake Enable
		GE04, 1,	// GPIO04 SCI/Wake Enable
		GE05, 1,	// GPIO05 SCI/Wake Enable
		GE06, 1,	// GPIO06 SCI/Wake Enable
		GE07, 1,	// GPIO07 SCI/Wake Enable
		GE08, 1,	// GPIO08 SCI/Wake Enable
		GE09, 1,	// GPIO09 SCI/Wake Enable
		GE10, 1,	// GPIO10 SCI/Wake Enable
		GE11, 1,	// GPIO11 SCI/Wake Enable
		GE12, 1,	// GPIO12 SCI/Wake Enable
		GE13, 1,	// GPIO13 SCI/Wake Enable
		GE14, 1,	// GPIO14 SCI/Wake Enable
		GE15, 1,	// GPIO15 SCI/Wake Enable
		Offset(0x42),	// General Purpose Control
		, 1,		// skip 1 bit
		GPEC, 1,	// SWGPE_CTRL
	}

	// GPIO IO mapped registers (0x1f.0 reg 0x48.l)
	OperationRegion(GPIO, SystemIO, DEFAULT_GPIOBASE, 0x6c)
	Field(GPIO, ByteAcc, NoLock, Preserve)
	{
		GU00, 8,	// GPIO Use Select
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
		GP06, 1,
		GP07, 1,
		GP08, 1,
		GP09, 1,
		GP10, 1,
		GP11, 1,
		GP12, 1,
		GP13, 1,
		GP14, 1,
		GP15, 1,
		GP16, 1,
		GP17, 1,
		GP18, 1,
		GP19, 1,
		GP20, 1,
		GP21, 1,
		GP22, 1,
		GP23, 1,
		GP24, 1,
		GP25, 1,
		GP26, 1,
		GP27, 1,
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
		GP33, 1,
		GP34, 1,
		GP35, 1,
		GP36, 1,
		GP37, 1,
		GP38, 1,
		GP39, 1,
		GP40, 1,
		GP41, 1,
		GP42, 1,
		GP43, 1,
		GP44, 1,
		GP45, 1,
		GP46, 1,
		GP47, 1,
		GP48, 1,
		GP49, 1,
		GP50, 1,
		GP51, 1,
		GP52, 1,
		GP53, 1,
		GP54, 1,
		GP55, 1,
		GP56, 1,
		GP57, 1,
		GP58, 1,
		GP59, 1,
		GP60, 1,
		GP61, 1,
		GP62, 1,
		GP63, 1,
		Offset(0x40),	// GPIO Use Select 3
		GU08, 8,
		GU09, 4,
		Offset(0x44),	// GPIO IO Select 3
		GIO8, 8,
		GIO9, 4,
		Offset(0x48),	// GPIO Level 3
		GP64, 1,
		GP65, 1,
		GP66, 1,
		GP67, 1,
		GP68, 1,
		GP69, 1,
		GP70, 1,
		GP71, 1,
		GP72, 1,
		GP73, 1,
		GP74, 1,
		GP75, 1,
	}


	// ICH7 Root Complex Register Block. Memory Mapped through RCBA)
	OperationRegion(RCRB, SystemMemory, CONFIG_FIXED_RCBA_MMIO_BASE, CONFIG_RCBA_LENGTH)
	Field(RCRB, DWordAcc, Lock, Preserve)
	{
		Offset(0x0000), // Backbone
		Offset(0x1000), // Chipset
		Offset(0x3000), // Legacy Configuration Registers
		Offset(0x3404), // High Performance Timer Configuration
		HPAS, 2,	// Address Select
		, 5,
		HPTE, 1,	// Address Enable
		Offset(0x3418), // FD (Function Disable)
		, 1,		// Reserved
		PCID, 1,	// PCI bridge disable
		SA1D, 1,	// SATA1 disable
		SMBD, 1,	// SMBUS disable
		HDAD, 1,	// Azalia disable
		, 8,		// Reserved
		EH2D, 1,	// EHCI #2 disable
		LPBD, 1,	// LPC bridge disable
		EH1D, 1,	// EHCI #1 disable
		RP1D, 1,	// Root Port 1 disable
		RP2D, 1,	// Root Port 2 disable
		RP3D, 1,	// Root Port 3 disable
		RP4D, 1,	// Root Port 4 disable
		RP5D, 1,	// Root Port 5 disable
		RP6D, 1,	// Root Port 6 disable
		RP7D, 1,	// Root Port 7 disable
		RP8D, 1,	// Root Port 8 disable
		TTRD, 1,	// Thermal sensor registers disable
		SA2D, 1,	// SATA2 disable
		Offset(0x3428),	// FD2 (Function Disable 2)
		BDFD, 1,	// Display BDF
		ME1D, 1,	// ME Interface 1 disable
		ME2D, 1,	// ME Interface 2 disable
		IDRD, 1,	// IDE redirect disable
		KTCT, 1,	// Keyboard Text redirect disable
	}
}

// High Definition Audio (Azalia) 0:1b.0
#include "audio.asl"

// PCI Express Ports 0:1c.x
#include <southbridge/intel/common/acpi/pcie.asl>

// USB EHCI 0:1d.0 and 0:1a.0, XHCI 0:14.0
#include "usb.asl"

// LPC Bridge 0:1f.0
#include "lpc.asl"

// SATA 0:1f.2, 0:1f.5
#include "sata.asl"

// SMBus 0:1f.3
#include <southbridge/intel/common/acpi/smbus.asl>

Method (_OSC, 4)
{
	/*
	 * Arg0 - A Buffer containing a UUID
	 * Arg1 - An Integer containing a Revision ID of the buffer format
	 * Arg2 - An Integer containing a count of entries in Arg3
	 * Arg3 - A Buffer containing a list of DWORD capabilities
	 */
	/* Check for XHCI */
	If (Arg0 == ToUUID("7c9512a9-1705-4cb4-af7d-506a2423ab71"))
	{
		Return (^XHC.POSC(Arg2, Arg3))
	}

	/* Check for PCIe */
	If (Arg0 == ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
	{
		/* Let OS control everything */
		Return (Arg3)
	}

	/* Else Return Unrecognized UUID */
	CreateDWordField (Arg3, 0, CDW1)
	Or (CDW1, 4, CDW1)
	Return (Arg3)

}
