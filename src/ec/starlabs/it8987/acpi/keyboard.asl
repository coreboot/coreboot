/* SPDX-License-Identifier: GPL-2.0-only */

Method(_Q80)				// Volume up
{
	Store ("-----> _Q80", Debug)
	Notify (\_SB.HIDD, 0xC4)
	Notify (\_SB.HIDD, 0xC5)
	Store ("<----- _Q80", Debug)
}

Method(_Q81)				// Volume down
{
	Store ("-----> _Q81", Debug)
	Notify (\_SB.HIDD, 0xC6)
	Notify (\_SB.HIDD, 0xC7)
	Store ("<----- _Q81", Debug)
}

Method(_Q99)				// Wireless mode
{
	Store ("-----> _Q99", Debug)
	\_SB.HIDD.HPEM(8)
	Store ("<----- _Q80", Debug)
}

Method(_Q06)				// Brightness decrease
{
	\_SB.PCI0.GFX0.DECB()
}

Method(_Q07)				// Brightness increase
{
	\_SB.PCI0.GFX0.INCB()
}

Method(_Q08)				// FN lock QEvent
{
	FNLC = FNST
}

Method(_Q54)				// Power Button Event
{
	Store ("-----> _Q54", Debug)
	If (CondRefOf (\_SB.PWRB))
	{
		Notify(\_SB.PWRB, 0x80)
	}
	Store ("<----- _Q54", Debug)
}

Method(_QD5)				// 10 second power button press
{
	Store ("-----> _QD5", Debug)
	\_SB.PWPR()
	Store ("<----- _QD5", Debug)
}

Method(_QD6)				// 10 second power button de-press
{
	Store ("-----> _QD6", Debug)
	\_SB.PWRR()
	Store ("<----- _QD6", Debug)
}
