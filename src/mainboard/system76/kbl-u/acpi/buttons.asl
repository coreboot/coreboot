/* SPDX-License-Identifier: GPL-2.0-only */

Device (PWRB)
{
	Name (_HID, EisaId ("PNP0C0C"))
	Name (_PRW, Package () { 0x13 /* GPP_C19 */, 3 })
}

Device (SLPB)
{
	Name (_HID, EisaId ("PNP0C0E"))
	Name (_PRW, Package () { 0x13 /* GPP_C19 */, 3 })
}
