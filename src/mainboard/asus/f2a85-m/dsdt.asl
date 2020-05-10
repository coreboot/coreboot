/* SPDX-License-Identifier: GPL-2.0-only */

/* DefinitionBlock Statement */
#include <acpi/acpi.h>
DefinitionBlock (
	"DSDT.AML",		/* Output filename */
	"DSDT",			/* Signature */
	0x02,			/* DSDT Revision, needs to be 2 for 64bit */
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00010001		/* OEM Revision */
	)
{	/* Start of ASL file */
	/* #include <arch/x86/acpi/debug.asl> */	/* Include global debug methods if needed */

	/* Globals for the platform */
	#include "acpi/mainboard.asl"

	/* Describe the USB Overcurrent pins */
	#include "acpi/usb_oc.asl"

	/* PCI IRQ mapping for the Southbridge */
	#include <southbridge/amd/agesa/hudson/acpi/pcie.asl>

	/* Describe the processor tree (\_SB) */
	#include <cpu/amd/agesa/family15tn/acpi/cpu.asl>

	/* Describe the supported Sleep States for this Southbridge */
	#include <southbridge/amd/common/acpi/sleepstates.asl>

	/* Describe the Sleep Methods (WAK, PTS, GTS, etc.) for this platform */
	#include "acpi/sleep.asl"

	Scope(\_SB) {
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
			#include <northbridge/amd/agesa/family15tn/acpi/northbridge.asl>

			/* Describe the AMD Fusion Controller Hub Southbridge */
			#include <southbridge/amd/agesa/hudson/acpi/fch.asl>

			/**
			 * TODO: The devices listed here (SBR0 and SBR1) do not appear to
			 *       be referenced anywhere and could possibly be removed.
			 */
			Device(SBR0) { /* PCIe 1x SB */
				Name(_ADR, 0x00150000)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT,0) {
					If(PMOD){ Return(ABR0) }   /* APIC mode */
					Return (PBR0)              /* PIC mode  */
				}
			}

			Device(SBR1) { /* Onboard network */
				Name(_ADR, 0x00150001)
				Name(_PRW, Package() {0x18, 4})
				Method(_PRT, 0) {
					If(PMOD){ Return(ABR1) }   /* APIC mode */
					Return (PBR1)              /* PIC mode  */
				}
			}
		}

		/* Describe PCI INT[A-H] for the Southbridge */
		#include <southbridge/amd/agesa/hudson/acpi/pci_int.asl>

	}   /* End Scope(_SB)  */

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
