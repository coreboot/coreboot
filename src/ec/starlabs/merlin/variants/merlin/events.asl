/* SPDX-License-Identifier: GPL-2.0-only */

Method (_Q0D)					// Event: Lid Opened
{
	\LIDS = LSTE
	Notify (LID0, 0x80)
}

Method (_Q0C)					// Event: Lid Closed
{
	\LIDS = LSTE
	Notify (LID0, 0x80)
}

Method (_Q0A)					// Event: AC Power Connected
{
	Notify (BAT0, 0x81)
	Notify (ADP1, 0x80)
}

Method (_Q0B, 0, NotSerialized)			// Event: AC Power Disconnected
{
	Notify (BAT0, 0x81)
	Notify (BAT0, 0x80)
}

Method (_Q05)					// Event: Backlight Brightness Down
{
	^^^^HIDD.HPEM (20)
}

Method (_Q06)					// Event: Backlight Brightness Up
{
	^^^^HIDD.HPEM (19)
}

Method (_Q87)					// Event: Function Lock
{
	FLKS = FLKA
}

Method (_Q88)					// Event: Trackpad Lock
{
	TPLS = TPLA
}
Method (_Q11)					// Event: Keyboard Backlight Brightness
{
	KLBC = KLBE
	KLSC = KLSE
}

Method (_Q99)					// Event: Airplane Mode
{
	^^^^HIDD.HPEM (8)
}

Method (_QD5)					// Event: 10 Second Power Button Pressed
{
	Store ("EC: 10 Second Power Button Pressed", Debug)
}

Method (_QD6)					// Event: 10 Second Power Button Released
{
	Store ("EC: 10 Second Power Button Release", Debug)
}

Method (_Q22, 0, NotSerialized)			// Event: CHARGER_T
{
	Store ("EC: CHARGER_T", Debug)
}

Method (_Q40)					// Event: AC_DC
{
	SMB2 = 0xC6
}

Method (_Q41)					// Event: DC_20_0
{
	SMB2 = 0xC7
}

Method (_Q42)					// Event: DC_60_20
{
	SMB2 = 0xC9
}

Method (_Q43)					// Event: DC_100_60
{
	SMB2 = 0xC9
}

Method (_Q44)					// Event: AC_ONLY
{
	SMB2 = 0xCA
}

Method (_Q80, 0, NotSerialized)			// Event: VOLUME_UP
{
	Store ("EC: VOLUME_UP", Debug)
}

Method (_Q81, 0, NotSerialized)			// Event: VOLUME_DOWN
{
	Store ("EC: VOLUME_DOWN", Debug)
}

Method (_Q54, 0, NotSerialized)			// Event: PWRBTN
{
	Store ("EC: PWRBTN", Debug)
}

Method (_QF0)					// Event: Temperature Report
{
	Store ("EC: Temperature Report", Debug)
}

Method (_QF1)					// Event: Temperature Trigger
{
	// Notify (SEN3, 0x90)
}

/*
 * The below events are unique to this platform.
 */

Method (_Q79, 0, NotSerialized)			// Event: USB Type-C
{
	Store ("EC: USB Type-C", Debug)
	UCEV()
}

Method (_Q85, 0, NotSerialized)			// Event: HOME
{
	Store ("EC: HOME", Debug)
}

Method (_Q01)					// Event: F1 Hot Key
{
	Store ("EC: F1", Debug)
}

Method (_Q02)					// Event: F2 Hot Key
{
	Store ("EC: F2", Debug)
}

Method (_Q03)					// Event: F3 Hot Key
{
	Store ("EC: F3", Debug)
}

Method (_Q04)					// Event: F4 Hot Key
{
	Store ("EC: F4", Debug)
}

Method (_Q08)					// Event: F5 Hot Key
{
	Store ("EC: F5", Debug)
}

Method (_Q09)					// Event: F6 Hot Key
{
	Store ("EC: F6", Debug)
}

Method (_Q07)					// Event: F7 Hot Key
{
	Store ("EC: F7", Debug)
}

Method (_Q10)					// Event: F10 Hot Key
{
	Store ("EC: F10", Debug)
}

Method (_Q12)					// Event: F12 Hot Key
{
	Store ("EC: F6", Debug)
}

Method (_Q0E, 0, NotSerialized)			// Event: SLEEP
{
	Store ("EC: SLEEP", Debug)
}

Method (_Q13, 0, NotSerialized)			// Event: BRIGHTNESS
{
	Store ("EC: BRIGHTNESS", Debug)
}

Method (_Q20, 0, NotSerialized)			// Event: CPU_T
{
	Store ("EC: CPU_T", Debug)
}

Method (_Q21, 0, NotSerialized)			// Event: SKIN_T
{
	Store ("EC: SKIN_T", Debug)
}

Method (_Q30, 0, NotSerialized)			// Event: THROT_OFF
{
	Store ("EC: THROT_OFF", Debug)
}

Method (_Q31, 0, NotSerialized)			// Event: THROT_LV1
{
	Store ("EC: THROT_LV1", Debug)
}

Method (_Q32, 0, NotSerialized)			// Event: THROT_LV2
{
	Store ("EC: THROT_LV2", Debug)
}

Method (_Q33, 0, NotSerialized)			// Event: THROT_LV3
{
	Store ("EC: THROT_LV3", Debug)
}

Method (_Q34, 0, NotSerialized)			// Event: THROT_LV4
{
	Store ("EC: THROT_LV4", Debug)
}

Method (_Q35, 0, NotSerialized)			// Event: THROT_LV5
{
	Store ("EC: THROT_LV5", Debug)
}

Method (_Q36, 0, NotSerialized)			// Event: THROT_LV6
{
	Store ("EC: THROT_LV6", Debug)
}

Method (_Q37, 0, NotSerialized)			// Event: THROT_LV7
{
	Store ("EC: THROT_LV7", Debug)
}

Method (_Q38, 0, NotSerialized)			// Event: CPU_DN_SPEED
{
	Store ("EC: CPU_DN_SPEED", Debug)
}

Method (_Q3C, 0, NotSerialized)			// Event: CPU_UP_SPEED
{
	Store ("EC: CPU_UP_SPEED", Debug)
}

Method (_Q3D, 0, NotSerialized)			// Event: CPU_TURBO_OFF
{
	Store ("EC: CPU_TURBO_OFF", Debug)
}

Method (_Q3E, 0, NotSerialized)			// Event: CPU_TURBO_ON
{
	Store ("EC: CPU_TURBO_ON", Debug)
}

Method (_Q3F, 0, NotSerialized)			// Event: SHUTDOWN
{
	Store ("EC: SHUTDOWN", Debug)
}

Method (_Q45)					// Event: SENSOR_T76
{
	SMB2 = 0xCB
}

Method (_Q48, 0, NotSerialized)			// Event: Fan Turbo On
{
	Store ("EC: Fan Turbo On", Debug)
}

Method (_Q49, 0, NotSerialized)			// Event: Fan Turbo Off
{
	Store ("EC: Fan Turbo Off", Debug)
}
