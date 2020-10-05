/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20171231	/* OEM Revision */
)
{
	#include "acpi/platform.asl"
	#include <cpu/intel/common/acpi/cpu.asl>
	#include <southbridge/intel/common/acpi/platform.asl>
	#include <southbridge/intel/bd82x6x/acpi/globalnvs.asl>
	#include <southbridge/intel/common/acpi/sleepstates.asl>

	Device (\_SB.PCI0)
	{
		#include <northbridge/intel/sandybridge/acpi/sandybridge.asl>
		#include <southbridge/intel/bd82x6x/acpi/pch.asl>
		Device (PCIB)
		{
			Name (_ADR, 0x001E0000)
			Name (_PRW, Package(){ 13, 4 })
			Method (_PRT)
			{
				If (PICM) {
					Return (Package() {
						Package() { 0x0003ffff, 0, 0, 0x17 },
					})
				}
				Return (Package() {
					Package() { 0x0003ffff, 0, \_SB.PCI0.LPCB.LNKH, 0 },
				})
			}
		}
	}

	Scope (\_SB.PCI0.PEGP.DEV0)
	{
		Name (_SUN, 7)
	}

	Scope (\_SB.PCI0.PEG1.DEV0)
	{
		Name (_SUN, 6)
	}

	Scope (\_SB.PCI0.PEG6.DEV0)
	{
		Name (_SUN, 5)
	}

	Scope (\_SB.PCI0.RP01)
	{
		Device (DEV0)
		{
			Name (_ADR, 0x00000000)
			Name (_SUN, 4)
		}
	}
}
