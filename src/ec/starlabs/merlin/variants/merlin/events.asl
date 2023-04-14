/* SPDX-License-Identifier: GPL-2.0-only */

Method (_Q05, 0, NotSerialized)			// Event: Backlight Brightness Down
{
	^^^^HIDD.HPEM (20)
}

Method (_Q06, 0, NotSerialized)			// Event: Backlight Brightness Up
{
	^^^^HIDD.HPEM (19)
}

Method (_Q0A, 0, NotSerialized)			// Event: AC Power Connected
{
	Notify (BAT0, 0x81)
	Notify (ADP1, 0x80)
}

Method (_Q0B, 0, NotSerialized)			// Event: AC Power Disconnected
{
	Notify (BAT0, 0x81)
	Notify (BAT0, 0x80)
}

Method (_Q0C, 0, NotSerialized)			// Event: Lid Opened or Closed
{
	\LIDS = LSTE
	Notify (LID0, 0x80)
}
