/* SPDX-License-Identifier: GPL-2.0-only */


Device (SIO1)
{
	Name (_HID, EISAID("PNP0A05"))
	Name (_UID, 1)

	Device (UAR1)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 1)
		Name(_DDN, "COM1")

		Method (_STA, 0)
		{
			// always enable for now
			Return (0x0f)
		}

		Method (_CRS, 0)
		{
			Return(ResourceTemplate() {
				IO(Decode16, 0x3f8, 0x3f8, 0x8, 0x8)
				IRQNoFlags() { 4 }
			})
		}
		// Some methods need an implementation here:
		// missing: _STA, _DIS, _CRS, _PRS,
		// missing: _SRS, _PS0, _PS3
	}

	Device (UAR2)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 2)
		Name(_DDN, "COM2")

		Method (_STA, 0)
		{
			// always enable for now
			Return (0x0f)
		}

		Method (_CRS, 0)
		{
			Return(ResourceTemplate() {
				IO(Decode16, 0x2f8, 0x2f8, 0x8, 0x8)
				IRQNoFlags() { 3 }
			})
		}
		// Some methods need an implementation here:
		// missing: _STA, _DIS, _CRS, _PRS,
		// missing: _SRS, _PS0, _PS3
	}
}
