/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel Lynx Point PCH support */

#if CONFIG(INTEL_LYNXPOINT_LP)
#define DEFAULT_PRW_VALUE	0x6d
#else
#define DEFAULT_PRW_VALUE	0x0d
#endif

Scope (\)
{
	// Root Complex Register Block
	OperationRegion (RCRB, SystemMemory, CONFIG_FIXED_RCBA_MMIO_BASE, CONFIG_RCBA_LENGTH)
	Field (RCRB, DWordAcc, Lock, Preserve)
	{
		Offset (0x3404), // High Performance Timer Configuration
		HPAS, 2,	// Address Select
		, 5,
		HPTE, 1,	// Address Enable
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
