/* SPDX-License-Identifier: GPL-2.0-only */
Name (FLAP, 0x40) /* Flag indicating device is in laptop mode */

/* Virtual events */
Name (VTBL, 0xcc) /* Tablet Mode */
Name (VLAP, 0xcd) /* Laptop Mode */

Device (VBTN)
{
	Name (_HID, "INT33D6")
	Name (_UID, 1)
	Name (_DDN, "Intel Virtual Button Driver")

	/*
	 * This method is called at driver probe time and must exist or
	 * the driver will not load.
	 */
	Method (VBDL)
	{
	}

	/*
	 * This method returns flags indicating tablet and dock modes.
	 * It is called at driver probe time so the OS knows what the
	 * state of the device is at boot.
	 */
	Method (VGBS)
	{
		Local0 = 0
		If (CKLP ()) {
			Local0 |= ^^FLAP
		}
		Return (Local0)
	}

	Method (_STA, 0)
	{
		Return (0xF)
	}

	Method (CKLP, 0)
	{
		/* 120 = GPP_D4 */
		If (\_SB.PCI0.GRXS (120)) {
			Printf ("VBTN: tablet mode")
			Return (0)	/* Tablet mode */
		} Else {
			Printf ("VBTN: laptop mode")
			Return (1)	/* Laptop mode */
		}
	}

	Method (NTFY, 0)
	{
		/* Notify the new state */
		If (CKLP ()) {
			Notify (^^VBTN, ^^VLAP)
		} Else {
			Notify (^^VBTN, ^^VTBL)
		}
	}
}
