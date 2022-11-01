/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/gpio.h>

Method (PGPM, 1, Serialized)
{
	For (Local0 = 0, Local0 < 6, Local0++)
	{
		\_SB.PCI0.CGPM (Local0, Arg0)
	}
}

/*
 * Method called from _PTS prior to system sleep state entry
 * Enables dynamic clock gating for all 5 GPIO communities
 */
Method (MPTS, 1, Serialized)
{
	\_SB.PCI0.LPCB.EC0.PTS (Arg0)
	PGPM (MISCCFG_GPIO_PM_CONFIG_BITS)
}

/*
 * Method called from _WAK prior to system sleep state wakeup
 * Disables dynamic clock gating for all 5 GPIO communities
 */
Method (MWAK, 1, Serialized)
{
	PGPM (0)
	\_SB.PCI0.LPCB.EC0.WAK (Arg0)
}

/*
 * S0ix Entry/Exit Notifications
 * Called from \_SB.PEPD._DSM
 */
Method (MS0X, 1, Serialized)
{
	If (Arg0 == 1) {
		/* S0ix Entry */
		PGPM (MISCCFG_GPIO_PM_CONFIG_BITS)
	} Else {
		/* S0ix Exit */
		PGPM (0)
	}
}
