/* SPDX-License-Identifier: GPL-2.0-only */

Device (PWRB)
{
	Name(_HID, EisaId("PNP0C0C"))

	// Wake from deep sleep via GPIO27
	Name(_PRW, Package(){27, 4})
}
