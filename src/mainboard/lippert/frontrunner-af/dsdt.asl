/* SPDX-License-Identifier: GPL-2.0-only */

/* DefinitionBlock Statement */
#include <acpi/acpi.h>
DefinitionBlock (
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	#include <acpi/dsdt_top.asl>

	#include <cpu/amd/agesa/family14/acpi/cpu.asl>

	#include <southbridge/amd/cimx/sb800/acpi/misc_io.asl>

	#include "acpi/routing.asl"

	/* Contains the supported sleep states for this chipset */
	#include <southbridge/amd/common/acpi/sleepstates.asl>

	/* Contains the Sleep methods (WAK, PTS, GTS, etc.) */
	#include "acpi/sleep.asl"

	#include "acpi/gpe.asl"

	#include "acpi/usb_oc.asl"

	/* System Bus */
	Scope(\_SB) { /* Start \_SB scope */
		#include <arch/x86/acpi/globutil.asl> /* global utility methods expected within the \_SB scope */

		/*  _SB.PCI0 */
		/* Note: Only need HID on Primary Bus */
		Device(PCI0) {

			/* Describe the AMD Northbridge */
			#include <northbridge/amd/agesa/family14/acpi/northbridge.asl>

			/* Operating System Capabilities Method */
			Method (_OSC, 4)
			{
				/* Check for PCI/PCI-X/PCIe GUID */
				If (Arg0 == ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))
				{
					/* Let OS control everything */
					Return (Arg3)
				}
				Else
				{
					/* Unrecognized UUID, so set bit 2 of Arg3 to 1 */
					CreateDWordField (Arg3, 0, CDW1)
					CDW1 |= 4
					Return (Arg3)
				}
			} /* End _OSC */

			Method(_BBN, 0) { /* Bus number = 0 */
				Return(0)
			}
			Method(_STA, 0) {
				Return(0x0B)     /* Status is visible */
			}

			Method(_PRT,0) {
				If(PICM){ Return(APR0) }   /* APIC mode */
				Return (PR0)                  /* PIC Mode */
			} /* end _PRT */



			/* Describe the Southbridge devices */

			#include <southbridge/amd/cimx/sb800/acpi/pcie.asl>

			Device(STCR) {
				Name(_ADR, 0x00110000)
				#include "acpi/sata.asl"
			} /* end STCR */

			#include <southbridge/amd/cimx/sb800/acpi/usb.asl>

			Device(SBUS) {
				Name(_ADR, 0x00140000)
			} /* end SBUS */

			#include <southbridge/amd/cimx/sb800/acpi/audio.asl>

			#include <southbridge/amd/cimx/sb800/acpi/lpc.asl>

			/* PCI bridge */
			Device(PIBR) {
				Name(_ADR, 0x00140004)
				Name(_PRW, Package() {0x18, 4})

				Method(_PRT, 0) {
					Return (PCIB)
				}
			} /* end HostPciBr */

			Device(ACAD) {
				Name(_ADR, 0x00140005)
			} /* end Ac97audio */

			Device(ACMD) {
				Name(_ADR, 0x00140006)
			} /* end Ac97modem */

			Name(CRES, ResourceTemplate() {
				/* Set the Bus number and Secondary Bus number for the PCI0 device
				 * The Secondary bus range for PCI0 lets the system
				 * know what bus values are allowed on the downstream
				 * side of this PCI bus if there is a PCI-PCI bridge.
				 * PCI buses can have 256 secondary buses which
				 * range from [0-0xFF] but they do not need to be
				 * sequential.
				 */
				WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
					0x0000,		/* address granularity */
					0x0000,		/* range minimum */
					0x00FF,		/* range maximum */
					0x0000,		/* translation */
					0x0100,		/* length */
					,, PSB0)	/* ResourceSourceIndex, ResourceSource, DescriptorName */

				IO(Decode16, 0x004E, 0x004E, 1, 2)	/* SIO config regs */
#if CONFIG(BOARD_LIPPERT_FRONTRUNNER_AF)
				IO(Decode16, 0x0E00, 0x0E00, 1, 0x80)	/* SIO runtime regs */
#endif
				IO(Decode16, 0x0CF8, 0x0CF8, 1,	8)

				WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x0000,			/* address granularity */
					0x0000,			/* range minimum */
					0x0CF7,			/* range maximum */
					0x0000,			/* translation */
					0x0CF8			/* length */
				)

				WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
					0x0000,			/* address granularity */
					0x0D00,			/* range minimum */
					0xFFFF,			/* range maximum */
					0x0000,			/* translation */
					0xF300			/* length */
				)

				Memory32Fixed(READONLY, 0x000A0000, 0x00020000, VGAM)	/* VGA memory space */
				/* memory space for PCI BARs below 4GB */
				Memory32Fixed(ReadOnly, 0x00000000, 0x00000000, MMIO)
			}) /* End Name(_SB.PCI0.CRES) */

			Method(_CRS, 0) {
				CreateDWordField(CRES, ^MMIO._BAS, MM1B)
				CreateDWordField(CRES, ^MMIO._LEN, MM1L)
				/*
				 * Declare memory between TOM1 and 4GB as available
				 * for PCI MMIO.
				 * Use ShiftLeft to avoid 64bit constant (for XP).
				 * This will work even if the OS does 32bit arithmetic, as
				 * 32bit (0x00000000 - TOM1) will wrap and give the same
				 * result as 64bit (0x100000000 - TOM1).
				 */
				MM1B = TOM1
				Local0 = 0x10000000 << 4
				Local0 -= TOM1
				MM1L = Local0

				Return(CRES) /* note to change the Name buffer */
			} /* end of Method(_SB.PCI0._CRS) */

		} /* End Device(PCI0)  */

		Device(PWRB) {	/* Start Power button device */
			Name(_HID, EISAID("PNP0C0C"))
			Name(_UID, 0xAA)
			Name(_PRW, Package () {3, 0x04})	/* wake from S1-S4 */
			Name(_STA, 0x0B) /* sata is invisible */
		}
	} /* End \_SB scope */
}
/* End of ASL file */
