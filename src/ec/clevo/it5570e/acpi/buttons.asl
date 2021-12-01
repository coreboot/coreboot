/* SPDX-License-Identifier: GPL-2.0-only */

Device (PWRB)
{
	Name (_HID, "PNP0C0C")
	Name (_PRW, Package () { EC_GPE_PWRB, 4 })
}

Device (SLPB)
{
	Name (_HID, "PNP0C0E")
	Name (_PRW, Package () { EC_GPE_SLPB, 3 })
}
