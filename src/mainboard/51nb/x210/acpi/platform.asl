/* SPDX-License-Identifier: GPL-2.0-or-later */

/* Enable ACPI _SWS methods */
#include <soc/intel/common/acpi/acpi_wake_source.asl>

/*
 * The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method (_PTS, 1)
{
}

/* The _WAK method is called on system wakeup */

Method (_WAK, 1)
{
	\LIDS = \_SB.PCI0.LPCB.EC.LIDC
	\PWRS = \_SB.PCI0.LPCB.EC.ACIN
	Return (Package (){ 0, 0 })
}
