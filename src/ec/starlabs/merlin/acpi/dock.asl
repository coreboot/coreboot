/* SPDX-License-Identifier: GPL-2.0-only */

Device (VBTN)
{
	Name (_HID, "INT33D6")
	Name (_UID, 1)
	Name (_DDN, "Intel Virtual Button Driver")

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0f)
	}

	Method (VBDL)
	{
	}

	Method (UPDK, 0, Serialized)
	{
		Local0 = VGBS()

		If (Local0 == 0) {
			Printf ("Tablet Mode")
			Notify (HIDD, 0xcc)	/* Tablet */
		} Else {
			Printf ("Docked")
			Notify (HIDD, 0xcd)	/* Docked */
		}
		Return (Local0)
	}

	Method (VGBS, 0)
	{
#if CONFIG(SYSTEM_TYPE_DETACHABLE)
		If (!GRXS (GPP_F15))
		{
			Return (0x40)
		}
#endif
		Return (0x00)
	}
}

Device (VBTO)
{
	Name (_HID, "INT33D3")
	Name (_CID, "PNP0C60")
	Name (_UID, 1)
	Name (_DDN, "Laptop/tablet mode indicator driver")

	Method (_STA, 0)
	{
		Return (0xf)
	}
}
