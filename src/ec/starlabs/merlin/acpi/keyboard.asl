/* SPDX-License-Identifier: GPL-2.0-only */

Method(_Q80)				// Volume up
{
	Printf ("-----> _Q80")
	Notify (\_SB.HIDD, 0xC4)
	Notify (\_SB.HIDD, 0xC5)
	Printf ("<----- _Q80")
}

Method(_Q81)				// Volume down
{
	Printf ("-----> _Q81")
	Notify (\_SB.HIDD, 0xC6)
	Notify (\_SB.HIDD, 0xC7)
	Printf ("<----- _Q81")
}

Method(_Q99)				// Wireless mode
{
	Printf ("-----> _Q99")
	^^^^HIDD.HPEM (8)
	Printf ("<----- _Q99")
}

Method(_Q06)				// Brightness decrease
{
	^^^^HIDD.HPEM (19)
}

Method(_Q05)				// Brightness increase
{
	^^^^HIDD.HPEM (20)
}

Method(_Q08)				// FN lock QEvent
{
	FNLC = FNST
}

Method(_Q54)				// Power Button Event
{
	Printf ("-----> _Q54")
	Printf ("<----- _Q54")
}

Method(_QD5)				// 10 second power button press
{
	Printf ("-----> _QD5")
	\_SB.PWPR()
	Printf ("<----- _QD5")
}

Method(_QD6)				// 10 second power button de-press
{
	Printf ("-----> _QD6")
	\_SB.PWRR()
	Printf ("<----- _QD6")
}
