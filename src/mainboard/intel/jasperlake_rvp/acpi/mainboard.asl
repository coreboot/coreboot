/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#if CONFIG(EC_GOOGLE_CHROMEEC)
Scope (\_SB)
{
	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
	}
}
#endif
