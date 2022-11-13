/* SPDX-License-Identifier: GPL-2.0-only */

// Scope is \_SB.PCI0.LPCB

Device (SIO)
{
	Name (_HID, EisaId("PNP0A05"))
	Name (_UID, 0)

#ifdef SIO_EC_ENABLE_PS2K
	Device (PS2K)		// Keyboard
	{
		Name (_UID, 0)
		Name (_HID, EISAID("PNP0303"))
		Name (_CID, EISAID("PNP030B"))

		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate()
		{
			FixedIO (0x60, 0x01)
			FixedIO (0x64, 0x01)
			IRQNoFlags () {1}
		})
	}
#endif

#ifdef SIO_ENABLE_PS2M
	Device (PS2M)		// Mouse
	{
		Name (_HID, EISAID("PNP0F13"))

		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate()
		{
			FixedIO (0x60, 0x01)
			FixedIO (0x64, 0x01)
			IRQNoFlags () {12}
		})
	}
#endif

}
