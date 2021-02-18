/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>

/*
 * S0ix Entry/Exit Notifications
 * Called from \_SB.PEPD._DSM
 */
Method (MS0X, 1, Serialized)
{
	If (Arg0 == 1) {
		/*
		 * On S0ix entry, clear the SLP_S0_GATE pin, so that the rest of
		 * the platform can transition to its low power state as well.
		 */
		\_SB.PCI0.CTXS(GPIO_SLP_S0_GATE);
	} Else {
		/*
		 * On S0ix exit, set the SLP_S0_GATE pin, so that the rest of
		 * the platform will resume from its low power state.
		 */
		\_SB.PCI0.STXS(GPIO_SLP_S0_GATE);
	}
}
