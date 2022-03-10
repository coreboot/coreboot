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

Method (_QA0, 0, NotSerialized)			// Event: AC Power Connected
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

Method (_Q04, 0, NotSerialized)			// Event: Trackpad Lock
{
	Printf ("EC: Trackpad Lock")
}

Method (_Q11, 0, NotSerialized)			// Event: Keyboard Backlight Brightness
{
	Printf ("EC: Keyboard Brightness")
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

Method (_Q40, 0, NotSerialized)			// Event: AC and DC Power
{
	SMB2 = 0xC6
}

Method (_Q41, 0, NotSerialized)			// Event: Battery Charge between 0% and 20%
{
	SMB2 = 0xC7
}

Method (_Q42, 0, NotSerialized)			// Event: Battery Charge between 20% and 60%
{
	SMB2 = 0xC8
}

Method (_Q43, 0, NotSerialized)			// Event: Battery Charge between 60% and 100%
{
	SMB2 = 0xC9
}

Method (_Q44, 0, NotSerialized)			// Event: AC Power Only
{
	SMB2 = 0xCA
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

Method (_QF0, 0, NotSerialized)			// Event: Temperature Report
{
	Printf ("EC: Temperature Report")
}

Method (_QF1, 0, NotSerialized)			// Event: Temperature Trigger
{
	// Notify (SEN3, 0x90)
}

/*
 * The below events are unique to this platform.
 */


Method (_Q02, 0, NotSerialized)			// Event: APP
{
	Printf ("EC: APP")
}

Method (_Q82, 0, NotSerialized)			// Event: MIC
{
	Printf ("EC: MIC")
}

Method (_Q83, 0, NotSerialized)			// Event: MUTE
{
	Printf ("EC: MUTE")
}
