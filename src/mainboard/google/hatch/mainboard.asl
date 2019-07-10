/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <intelblocks/gpio.h>

Method (LOCL, 1, Serialized)
{
	For (Local0 = 0, Local0 < 5, Local0++)
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
	LOCL (MISCCFG_ENABLE_GPIO_PM_CONFIG)
}

/*
 * Method called from _WAK prior to system sleep state wakeup
 * Disables dynamic clock gating for all 5 GPIO communities
 */
Method (MWAK, 1, Serialized)
{
	LOCL (0)
}

/*
 * S0ix Entry/Exit Notifications
 * Called from \_SB.LPID._DSM
 */
Method (MS0X, 1, Serialized)
{
	If (Arg0 == 1) {
		/* S0ix Entry */
		LOCL (MISCCFG_ENABLE_GPIO_PM_CONFIG)
	} Else {
		/* S0ix Exit */
		LOCL (0)
	}
}
