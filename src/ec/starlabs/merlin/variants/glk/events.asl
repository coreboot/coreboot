/* SPDX-License-Identifier: GPL-2.0-only */

Method (_Q0D, 0, NotSerialized)			// Event: Lid Opened
{
	\LIDS = LSTE
	Notify (LID0, 0x80)
}

Method (_Q0C, 0, NotSerialized)			// Event: Lid Closed
{
	\LIDS = LSTE
	Notify (LID0, 0x80)
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

Method (_Q06, 0, NotSerialized)			// Event: Backlight Brightness Down
{
	^^^^HIDD.HPEM (20)
}

Method (_Q07, 0, NotSerialized)			// Event: Backlight Brightness Up
{
	^^^^HIDD.HPEM (19)
}

Method (_Q08, 0, NotSerialized)			// Event: Function Lock
{
	Printf ("EC: Function Lock")
}
Method (_Q99, 0, NotSerialized)			// Event: Airplane Mode
{
	^^^^HIDD.HPEM (8)
}

Method (_QD5, 0, NotSerialized)			// Event: 10 Second Power Button Pressed
{
	Notify (HIDD, 0xCE)
}

Method (_QD6, 0, NotSerialized)			// Event: 10 Second Power Button Released
{
	Notify (HIDD, 0xCF)
}

Method (_Q22, 0, NotSerialized)			// Event: CHARGER_T
{
	Printf ("EC: CHARGER_T")
}

Method (_Q80, 0, NotSerialized)			// Event: Volume Up
{
	Printf ("EC: VOLUME_UP")
}

Method (_Q81, 0, NotSerialized)			// Event: Volume Down
{
	Printf ("EC: VOLUME_DOWN")
}

Method (_Q54, 0, NotSerialized)			// Event: Power Button Press
{
	Printf ("EC: PWRBTN")
}

Method (_Q60, 0, NotSerialized)			// Event: Trackpad Enable
{
	Printf ("EC: Trackpad Enable")
	SPC0 (0xD0C80610, 0x40800102)
}

Method (_Q61, 0, NotSerialized)			// Event: Trackpad Disable
{
	Printf ("EC: Trackpad Disable")
	SPC0 (0xD0C80610, 0x40800200)
}

Method (_QF0, 0, NotSerialized)			// Event: Temperature Report
{
	Printf ("EC: Temperature Report")
}

Method (_QF1, 0, NotSerialized)			// Event: Temperature Trigger
{
	// Notify (SEN3, 0x90)
}
