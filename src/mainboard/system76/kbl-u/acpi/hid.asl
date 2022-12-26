/* SPDX-License-Identifier: GPL-2.0-only */

Device (HIDD)
{
	Name (_HID, "INT33D5")
	Name (HBSY, 0)
	Name (HIDX, 0)
	Name (HRDY, 0)

	Method (HDEM, 0, Serialized)
	{
		HBSY = 0
		Return (HIDX)
	}

	Method (HDMM, 0, Serialized)
	{
		Return (0)
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
		Local0 = 0
		While ((Local0 < 0xFA) && HBSY)
		{
			Sleep (0x04)
			Local0++
		}

		If (HBSY == 1)
		{
			HBSY = 0
			HIDX = 0
			Return (1)
		}
		Else
		{
			Return (0)
		}
	}
}
