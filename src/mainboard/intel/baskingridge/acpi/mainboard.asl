/* SPDX-License-Identifier: GPL-2.0-only */

Device (PWRB)
{
	Name(_HID, EisaId("PNP0C0C"))

	// Wake
	Name(_PRW, Package(){0x1d, 0x05})
}
