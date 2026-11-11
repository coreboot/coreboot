/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB)
{
	/*
	 * Chrome EC Keyboard Backlight interface
	 */
	Device (KBLT)
	{
		Name (_HID, "GOOG0002")
		Name (_UID, 1)

		/*
		 * Presence for _STA. Default to present when this device is
		 * compiled in via EC_GOOGLE_CHROMEEC_KEYBOARD_BACKLIGHT.
		 * KBLS is set via SSDT after querying the EC using host commands.
		 */
		Name (KBLS, 0xf)

		Method (_STA, 0, NotSerialized)
		{
			Return (KBLS)
		}

		/* Read current backlight value */
		Method (KBQC, 0, NotSerialized)
		{
			Return (\_SB.PCI0.LPCB.EC0.KBLV)
		}

		/* Write new backlight value */
		Method (KBCM, 1, NotSerialized)
		{
			\_SB.PCI0.LPCB.EC0.KBLV = Arg0
		}
	}
}
