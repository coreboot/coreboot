/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB) {
	Device (PWRB)
	{
		Name(_HID, EisaId("PNP0C0C"))

		// SIO3_WAKESCI# is GPIO14
		Name(_PRW, Package(){0x1d, 0x05})
	}
}
