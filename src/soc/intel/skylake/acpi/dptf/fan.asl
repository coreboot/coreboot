/* SPDX-License-Identifier: GPL-2.0-only */

Device (TFN1)
{
	Name (_HID, "INT3404")
	Name (_UID, 0)
	Name (_STR, Unicode("Fan Control"))

	/* _FIF: Fan Information */
	Name (_FIF, Package ()
	{
		0,	// Revision
		1,	// Fine Grained Control
		2,	// Step Size
		0	// No Low Speed Notification
	})

	/* Return Fan Performance States defined by mainboard */
	Method (_FPS)
	{
		Return (\_SB.DFPS)
	}

	Name (TFST, Package ()
	{
		0,	// Revision
		0x00,	// Control
		0x00	// Speed
	})

	/* _FST: Fan current Status */
	Method (_FST, 0, Serialized,,PkgObj)
	{
		/* Fill in TFST with current control. */
		TFST [1] = \_SB.PCI0.LPCB.EC0.FAND
		Return (TFST)
	}

	/* _FSL: Fan Speed Level */
	Method (_FSL, 1, Serialized)
	{
		\_SB.PCI0.LPCB.EC0.FAND = Arg0
	}

	Method (_STA)
	{
		If (\DPTE == 1)
		{
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}
}
