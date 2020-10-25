/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel Lynx Point PCH support */

#if CONFIG(INTEL_LYNXPOINT_LP)
#define DEFAULT_PRW_VALUE	0x6d
#else
#define DEFAULT_PRW_VALUE	0x0d
#endif

Scope (\)
{
	// Return TRUE if chipset is LynxPoint-LP
	Method (ISLP, 0, NotSerialized)
	{
		Return (CONFIG(INTEL_LYNXPOINT_LP))
	}

	// IO-Trap at 0x800. This is the ACPI->SMI communication interface.
	OperationRegion (IO_T, SystemIO, 0x800, 0x10)
	Field (IO_T, ByteAcc, NoLock, Preserve)
	{
		Offset (0x8),
		TRP0, 8		// IO-Trap at 0x808
	}

	// Root Complex Register Block
	OperationRegion (RCRB, SystemMemory, DEFAULT_RCBA, 0x4000)
	Field (RCRB, DWordAcc, Lock, Preserve)
	{
		Offset (0x3404), // High Performance Timer Configuration
		HPAS, 2,	// Address Select
		, 5,
		HPTE, 1,	// Address Enable
		Offset (0x3418), // FD (Function Disable)
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
		Offset (0x3428), // FD2 (Function Disable 2)
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

// USB EHCI 0:1d.0 and 0:1a.0
#include "ehci.asl"

// USB XHCI 0:14.0
#include "xhci.asl"

// LPC Bridge 0:1f.0
#include "lpc.asl"

// SATA 0:1f.2, 0:1f.5
#include "sata.asl"

// SMBus 0:1f.3
#include <southbridge/intel/common/acpi/smbus.asl>

// Serial IO
#if CONFIG(INTEL_LYNXPOINT_LP)
#include "serialio.asl"
#endif

Method (_OSC, 4)
{
	/* Check for proper GUID */
	If (Arg0 == ToUUID ("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
	{
		/* Let OS control everything */
		Return (Arg3)
	}
	Else
	{
		/* Unrecognized UUID */
		CreateDWordField (Arg3, 0, CDW1)
		CDW1 |= 4
		Return (Arg3)
	}
}
