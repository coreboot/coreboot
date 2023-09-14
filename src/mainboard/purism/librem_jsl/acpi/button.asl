/* SPDX-License-Identifier: GPL-2.0-only */

Device (PWRB)
{
	Name (_HID, EisaId ("PNP0C0C"))
	Name (_PRW, Package () { EC_GPE_SWI, 3 })
}
