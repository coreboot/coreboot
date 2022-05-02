/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/include/commonlib/console/post_codes.h>

External(\_SB.MPTS, MethodObj)
External(\_SB.MWAK, MethodObj)
External(\_SB.PCI0.EGPM, MethodObj)
External(\_SB.PCI0.RGPM, MethodObj)

#include <arch/x86/acpi/post.asl>

/*
 * The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method (_PTS, 1)
{
	DBG0 = POST_OS_ENTER_PTS

	If (CondRefOf (\_SB.MPTS))
	{
		\_SB.MPTS (Arg0)
	}
	/*
	 * Save the current PM bits then
	 * enable GPIO PM with MISCCFG_GPIO_PM_CONFIG_BITS
	 */
	If (CondRefOf (\_SB.PCI0.EGPM))
	{
		\_SB.PCI0.EGPM ()
	}
}

/* The _WAK method is called on system wakeup */

Method (_WAK, 1)
{
	DBG0 = POST_OS_ENTER_WAKE

	If (CondRefOf (\_SB.MWAK))
	{
		\_SB.MWAK (Arg0)
	}
	/* Restore GPIO all Community PM */
	If (CondRefOf (\_SB.PCI0.RGPM))
	{
		\_SB.PCI0.RGPM ()
	}

	Return (Package(){0,0})
}
