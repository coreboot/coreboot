/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/iomap.h>

/* PCI IRQ assignment */
#include "pci_irqs.asl"

#define DEFAULT_PRW_VALUE	0x6d

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

	/*
	 * Check PCH type
	 * Return 1 if PCH is WildcatPoint
	 * Return 0 if PCH is LynxPoint
	 */
	Method (ISWP)
	{
		Local0 = \_SB.PCI0.LPCB.PDID & 0xfff0
		If (Local0 == 0x9cc0) {
			Return (1)
		} Else {
			Return (0)
		}
	}
}

// High Definition Audio (Azalia) 0:1b.0
#include <southbridge/intel/lynxpoint/acpi/audio.asl>

// ADSP/SST 0:13.0
#include "adsp.asl"

// PCI Express Ports 0:1c.x
#include <southbridge/intel/common/acpi/pcie.asl>

// USB EHCI 0:1d.0
#include <southbridge/intel/lynxpoint/acpi/ehci.asl>

// USB XHCI 0:14.0
#include "xhci.asl"

// LPC Bridge 0:1f.0
#include "lpc.asl"

// SATA 0:1f.2
#include <southbridge/intel/lynxpoint/acpi/sata.asl>

// SMBus 0:1f.3
#include <southbridge/intel/common/acpi/smbus.asl>

// Serial IO
#include "serialio.asl"

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
