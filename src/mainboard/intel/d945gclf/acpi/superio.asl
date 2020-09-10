/* SPDX-License-Identifier: GPL-2.0-only */

Device (SIO1)
{
	Name (_HID, EISAID("PNP0A05"))
	Name (_UID, 1)

	Device (UAR1)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 1)

		// Some methods need an implementation here:
		// missing: _STA, _DIS, _CRS, _PRS,
		// missing: _SRS, _PS0, _PS3
	}

	Device (UAR2)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 2)

		// Some methods need an implementation here:
		// missing: _STA, _DIS, _CRS, _PRS,
		// missing: _SRS, _PS0, _PS3
	}
}
