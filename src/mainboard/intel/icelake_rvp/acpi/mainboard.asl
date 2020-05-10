/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(EC_GOOGLE_CHROMEEC)
Scope (\_SB)
{
	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
	}
}
#endif
