/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/common/acpi/platform.asl>

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

Scope (\_SB)
{
	Method (_SWS)
	{
		/* Index into PM1 for device that caused wake */
		Return (\PM1I)
	}
}

Scope (\_GPE)
{
	Method (_SWS)
	{
		/* Index into GPE for device that caused wake */
		Return (\GPEI)
	}
}
