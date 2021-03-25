/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Configure GPIO Power Management bits
 *
 * Arg0: GPIO community index
 * Arg1: PM bits in MISCCFG
 */
Method (CGPM, 2, Serialized)
{
	Local0 = GPID (Arg0)
	If (Local0 != 0) {
		/* Mask off current PM bits */
		PCRA (Local0, GPIO_MISCCFG, ~MISCCFG_GPIO_PM_CONFIG_BITS)
		/* Mask in requested bits */
		PCRO (Local0, GPIO_MISCCFG,  Arg1 & MISCCFG_GPIO_PM_CONFIG_BITS)
	}
}
