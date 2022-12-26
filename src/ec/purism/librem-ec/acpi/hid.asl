/* SPDX-License-Identifier: GPL-2.0-only */

Device (HIDD)
{
	Name (_HID, "INT33D5")
	Name (HBSY, Zero)
	Name (HIDX, Zero)
	Name (HRDY, Zero)

	Method (HDEM, 0, Serialized)
	{
		HBSY = Zero
		Return (HIDX)
	}

	Method (HDMM, 0, Serialized)
	{
		Return (Zero)
	}

	Method (HDSM, 1, Serialized)
	{
		HRDY = Arg0
	}

	Method (HPEM, 1, Serialized)
	{
		HBSY = 1
		HIDX = Arg0

		Notify (HIDD, 0xC0)
		Local0 = Zero
		While ((Local0 < 0xFA) && HBSY)
		{
			Sleep (0x04)
			Local0++
		}

		If (HBSY == 1)
		{
			HBSY = Zero
			HIDX = Zero
			Return (1)
		}
		Else
		{
			Return (Zero)
		}
	}
}
