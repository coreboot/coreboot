/* SPDX-License-Identifier: GPL-2.0-or-later */

/* Enable ACPI _SWS methods */
#include <soc/intel/common/acpi/acpi_wake_source.asl>

Name (_S0, Package (0x04) // mandatory system state
{
	0x00, 0x00, 0x00, 0x00
})

Name (_S5, Package (0x04)  // mandatory system state
{
	0x07, 0x00, 0x00, 0x00
})

/* The APM port can be used for generating software SMIs */
OperationRegion (APMP, SystemIO, 0xb2, 2)
Field (APMP, ByteAcc, NoLock, Preserve)
{
	APMC, 8,	// APM command
	APMS, 8		// APM status
}

#include <arch/x86/acpi/post.asl>

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
