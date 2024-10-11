/* SPDX-License-Identifier: GPL-2.0-only */

Method (_Q05, 0, NotSerialized)			// Event: Backlight Brightness Down
{
	^^^^HIDD.HPEM (20)
}

Method (_Q06, 0, NotSerialized)			// Event: Backlight Brightness Up
{
	^^^^HIDD.HPEM (19)
}

Method (_Q0A, 0, NotSerialized)			// Event: Charger Status Update
{
	Notify (ADP1, 0x80)
}

Method (_Q0B, 0, NotSerialized)			// Event: Battery Information Update
{
	Notify (BAT0, 0x80)
}

Method (_Q0C, 0, NotSerialized)			// Event: Lid Opened or Closed
{
	\LIDS = ECRD(RefOf(LSTE))
	Notify (LID0, 0x80)
}
