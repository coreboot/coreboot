/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Onboard CPLD
 */
Scope (\_SB.PCI0.LPCB)
{
	Device (CPLD)		/* Onboard CPLD */
	{
		Name(_HID, EISAID("PNP0C01"))
		Name(_CRS, ResourceTemplate()
		{
			/* Reserve 0x280 to 0x2BF for the CPLD */
			FixedIO (0x0280, 0x40)
			IRQNoFlags () {7}
		})
	}
}
