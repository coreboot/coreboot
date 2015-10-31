/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

/* DefinitionBlock Statement */
DefinitionBlock (
	"DSDT.AML",	/* Output filename */
	"DSDT",		/* Signature */
	0x02,		/* DSDT Revision, needs to be 2 for 64bit */
	"AMD   ",	/* OEMID */
	"COREBOOT",	/* TABLE ID */
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	/* #include <arch/x86/acpi/debug.asl> */	/* Include global debug methods if needed */

	/* Globals for the platform */
	#include "acpi/mainboard.asl"

	/* Describe the USB Overcurrent pins */
	#include "acpi/usb_oc.asl"

	/* PCI IRQ mapping for the Southbridge */
	#include <southbridge/amd/agesa/hudson/acpi/pcie.asl>

	/* Describe the processor tree (\_PR) */
	#include <cpu/amd/agesa/family16kb/acpi/cpu.asl>

	/* Contains the supported sleep states for this chipset */
	#include <southbridge/amd/agesa/hudson/acpi/sleepstates.asl>

	/* Contains the Sleep methods (WAK, PTS, GTS, etc.) */
	#include "acpi/sleep.asl"

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
	 	/* global utility methods expected within the \_SB scope */
		#include <arch/x86/acpi/globutil.asl>

		/* Describe IRQ Routing mapping for this platform (within the \_SB scope) */
		#include "acpi/routing.asl"

		Device(PWRB) {
			Name(_HID, EISAID("PNP0C0C"))
			Name(_UID, 0xAA)
			Name(_PRW, Package () {3, 0x04})
			Name(_STA, 0x0B)
		}

		Device(PCI0) {
			/* Describe the AMD Northbridge */
			#include <northbridge/amd/agesa/family16kb/acpi/northbridge.asl>

			/* Describe the AMD Fusion Controller Hub Southbridge */
			#include <southbridge/amd/agesa/hudson/acpi/fch.asl>
		}

		/* Describe PCI INT[A-H] for the Southbridge */
		#include <southbridge/amd/agesa/hudson/acpi/pci_int.asl>

	} /* End \_SB scope */

	/* Describe SMBUS for the Southbridge */
	#include <southbridge/amd/agesa/hudson/acpi/smbus.asl>

	/* Define the General Purpose Events for the platform */
	#include "acpi/gpe.asl"

	/* Define the Thermal zones and methods for the platform */
	#include "acpi/thermal.asl"

	/* Define the System Indicators for the platform */
	#include "acpi/si.asl"
}
/* End of ASL file */
