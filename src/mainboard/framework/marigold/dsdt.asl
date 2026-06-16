/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <mainboard/ec.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	COREBOOT_OEM_REVISION
)
{
	#include <acpi/dsdt_top.asl>
	#include <soc/intel/common/block/acpi/acpi/platform.asl>
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0) {
		#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
		#include <soc/intel/meteorlake/acpi/southbridge.asl>
		#include <soc/intel/meteorlake/acpi/tcss.asl>
		#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	/* Framework EC over eSPI */
	Scope (\_SB.PCI0.LPCB)
	{
		/* PS/2 keyboard + mouse, and EC host-command/memmap I/O reservations */
		#include <ec/google/chromeec/acpi/superio.asl>
		/* EC0: lid switch, AC adapter, battery, cros_ec command device */
		#include <ec/google/chromeec/acpi/ec.asl>
	}

	/*
	 * Clear ACPI driver ready before entering suspend, same as the vendor
	 * firmware, so the EC returns to preOS mode across the power transition.
	 */
	Method (\_SB.MPTS, 1, Serialized)
	{
		If (Arg0) {
			\_SB.PCI0.LPCB.EC0.ADRD = 0
		}
	}

	/*
	 * Signal ACPI driver ready to EC again, after resume from suspend,
	 * same as on boot, to make sure function keys, etc. are working.
	 */
	Method (\_SB.MWAK, 1, Serialized)
	{
		If ((Arg0 == 0x03) || (Arg0 == 0x04)) {
			\_SB.PCI0.LPCB.EC0.ADRD = 1
		}
	}
}
