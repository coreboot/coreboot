/* SPDX-License-Identifier: GPL-2.0-only */

/* Windows drivers require proper scoping to access child devices */
Scope (\_SB.CREC)
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

/*
 * Compatibility shim for older Linux drivers that hardcode \_SB.KBLT.
 * Keep _STA clear so this node is not enumerated (avoids a second
 * GOOG0002 bind); methods forward to the real device under CREC.
 */
Scope (\_SB)
{
	Device (KBLT)
	{
		/* _HID required by iASL; _STA 0 keeps this node from enumerating. */
		Name (_HID, "GOOG0002")
		Name (_UID, 2)

		Method (_STA, 0, NotSerialized)
		{
			Return (0x0)
		}

		Method (KBQC, 0, NotSerialized)
		{
			Return (\_SB.CREC.KBLT.KBQC())
		}

		Method (KBCM, 1, NotSerialized)
		{
			\_SB.CREC.KBLT.KBCM(Arg0)
		}
	}
}
