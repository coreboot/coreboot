/* SPDX-License-Identifier: GPL-2.0-only */

Device(LID)
{
	Name(_HID, "PNP0C0D")

	Method(_LID, 0, NotSerialized)
	{
		Return (LIDS)
	}
}
