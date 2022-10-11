/* SPDX-License-Identifier: GPL-2.0-only */

Method (_Q01, 0, NotSerialized)			// Event: F1 Hot Key
{
	Printf ("EC: F1")
}

Method (_Q02, 0, NotSerialized)			// Event: F2 Hot Key
{
	Printf ("EC: F2")
}

Method (_Q03, 0, NotSerialized)			// Event: F3 Hot Key
{
	Printf ("EC: F3")
}

Method (_Q04, 0, NotSerialized)			// Event: F4 Hot Key
{
	Printf ("EC: F4")
}

Method (_Q05, 0, NotSerialized)			// Event: F5 Hot Key
{
	Printf ("EC: F5")
}

Method (_Q06, 0, NotSerialized)			// Event: F6 Hot Key
{
	Printf ("EC: F6")
}

Method (_Q07, 0, NotSerialized)			// Event: F7 Hot Key
{
	Printf ("EC: F7")
}

Method (_Q08, 0, NotSerialized)			// Event: F8 Hot Key
{
	Printf ("EC: F8")
}

Method (_Q09, 0, NotSerialized)			// Event: F9 Hot Key
{
	Printf ("EC: F9")
}

Method (_Q10, 0, NotSerialized)			// Event: F10 Hot Key
{
	Printf ("EC: F10")
}

Method (_Q12, 0, NotSerialized)			// Event: F12 Hot Key
{
	Printf ("EC: F12")
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

Method (_Q0C, 0, NotSerialized)			// Event: Lid Closed
{
	\LIDS = LSTE
	Notify (LID0, 0x80)
}

Method (_Q0D, 0, NotSerialized)			// Event: Lid Open
{
	\LIDS = LSTE
	Notify (LID0, 0x80)
}

Method (_Q0E, 0, NotSerialized)			// Event: SLEEP
{
	Printf ("EC: SLEEP")
}

Method (_Q13, 0, NotSerialized)			// Event: BRIGHTNESS
{
	Printf ("EC: BRIGHTNESS")
}

Method (_Q20, 0, NotSerialized)			// Event: CPU_T
{
	Printf ("EC: CPU_T")
}

Method (_Q21, 0, NotSerialized)			// Event: SKIN_T
{
	Printf ("EC: SKIN_T")
}

Method (_Q22, 0, NotSerialized)			// Event: CHARGER_T
{
	Printf ("EC: CHARGER_T")
}

Method (_Q30, 0, NotSerialized)			// Event: THROT_OFF
{
	Printf ("EC: THROT_OFF")
}

Method (_Q31, 0, NotSerialized)			// Event: THROT_LV1
{
	Printf ("EC: THROT_LV1")
}

Method (_Q32, 0, NotSerialized)			// Event: THROT_LV2
{
	Printf ("EC: THROT_LV2")
}

Method (_Q33, 0, NotSerialized)			// Event: THROT_LV3
{
	Printf ("EC: THROT_LV3")
}

Method (_Q34, 0, NotSerialized)			// Event: THROT_LV4
{
	Printf ("EC: THROT_LV4")
}

Method (_Q35, 0, NotSerialized)			// Event: THROT_LV5
	Printf ("EC: THROT_LV5")
}

Method (_Q36, 0, NotSerialized)			// Event: THROT_LV6
{
	Printf ("EC:THROT_LV6")
}

Method (_Q37, 0, NotSerialized)			// Event: THROT_LV7
{
	Printf ("EC: THROT_LV7")
}

Method (_Q3B, 0, NotSerialized)			// Event: CPU_DN_SPEED
	Printf ("EC: CPU_DN_SPEED")
}

Method (_Q3C, 0, NotSerialized)			// Event: CPU_UP_SPEED
{
	Printf ("EC: CPU_UP_SPEED")
}

Method (_Q3D, 0, NotSerialized)			// Event: CPU_TURBO_OFF
{
	Printf ("EC: CPU_TURBO_OFF")
}

Method (_Q3E, 0, NotSerialized)			// Event: CPU_TURBO_ON
{
	Printf ("EC: CPU_TURBO_ON")
}

Method (_Q3F, 0, NotSerialized)			// Event: SHUTDOWN
	Printf ("EC: SHUTDOWN")
}

Method (_Q54, 0, NotSerialized)			// Event: Power Button Press
{
	Printf ("EC: PWRBTN")
}

Method (_Q79, 0, NotSerialized)			// Event: USB Type-C
{
	Printf ("EC: USB Type-C")
	UCEV()
}

Method (_Q80, 0, NotSerialized)			// Event: Volume Up
{
	Printf ("EC:VOLUME_UP")
}

Method (_Q81, 0, NotSerialized)			// Event: Volume Down
{
	Printf ("EC: VOLUME_DOWN")
}

Method (_Q85, 0, NotSerialized)			// Event: HOME
{
	Printf ("EC: HOME")
}
