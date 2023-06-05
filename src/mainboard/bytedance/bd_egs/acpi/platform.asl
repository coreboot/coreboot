/* SPDX-License-Identifier: GPL-2.0-only */

/* Enable ACPI _SWS methods */
#include <soc/intel/common/acpi/acpi_wake_source.asl>
#include <southbridge/intel/common/acpi/sleepstates.asl>

/* Port 80 POST */
OperationRegion (DBG0, SystemIO, 0x80, 0x02)
Field (DBG0, ByteAcc, Lock, Preserve)
{
	IO80, 8,
	IO81, 8
}

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
	Return (Package (){ 0, 0 })
}
