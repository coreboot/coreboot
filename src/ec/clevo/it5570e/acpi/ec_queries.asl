/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Note:
 * The code contains all queries/events that are known to be implemented by the EC firmware.
 * Not all events are known yet, though.
 */

Method (_Q0A)
{
	Printf ("EC: _Q0A: Toggle touchpad, SCIE=0x%o, state=%o",
		ToHexString (SCIE), ToDecimalString(SCIE & 1))
}

Method (_Q0B)
{
	Printf ("EC: _Q0B: LCD off, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q0C)
{
	Printf ("EC: _Q0C: Toggle mute, SCIE=0x%o, state=%o",
		ToHexString (SCIE), ToDecimalString(SCIE & 1))
}

Method (_Q0E)
{
	Printf ("EC: _Q0E: Decrease volume")
}

Method (_Q0F)
{
	Printf ("EC: _Q0F: Increase volume")
}

Method (_Q10)
{
	Printf ("EC: _Q10, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q11)
{
	Printf ("EC: _Q11: Decrease brightness")

	If (CondRefOf (\_SB.PCI0.GFX0.LCD0)) {
		Notify (\_SB.PCI0.GFX0.LCD0, 0x87)
	}
}

Method (_Q12)
{
	Printf ("EC: _Q12: Increase brightness")

	If (CondRefOf (\_SB.PCI0.GFX0.LCD0)) {
		Notify (\_SB.PCI0.GFX0.LCD0, 0x86)
	}
}

Method (_Q13)
{
	Printf ("EC: _Q13: Toggle camera, SCIE=0x%o, state=%o",
		ToHexString (SCIE), ToDecimalString(SCIE & 1))
}

Method (_Q14)
{
	Printf ("EC: _Q14: Toggle airplane mode, SCIE=0x%o", ToHexString (SCIE))

	\_SB.HIDD.HPEM (8)
}

Method (_Q15)
{
	Printf ("EC: _Q15: Sleep button")

	Notify (\_SB.SLPB, 0x80)
}

Method (_Q16)
{
	Printf ("EC: _Q16: Power event (AC/BAT0)")

	Notify (AC, 0x80) /* status change */
	If (BAT0)
	{
		Notify (\_SB.BAT0, 0x80) /* status change */
		Notify (\_SB.BAT0, 0x81) /* information change */
	}
}

Method (_Q17)
{
	Printf ("EC: _Q17: Battery presence change, state=%o", ToDecimalString (BAT0))

	Notify (\_SB.BAT0, 0x81) /* information change */
}

Method (_Q19)
{
	Printf ("EC: _Q19: Battery critical")

	Notify (\_SB.BAT0, 0x80) /* status change */
}

Method (_Q1A)
{
	Printf ("EC: _Q1A: Wake event, WFNO=0x%o", ToHexString (WFNO))

	Switch (ToInteger (WFNO))
	{
		Case (0x01)
		{
			Printf ("EC: Wake reason: Lid")
			Notify (\_SB.LID, 0x02) /* wake */
		}

		Case (0x04)
		{
			Printf ("EC: Wake reason: Sleep button")
			Notify (\_SB.SLPB, 0x02) /* wake */
		}

		Case (0x05)
		{
			Printf ("EC: Wake reason: Timer")
			Notify (\_SB.PWRB, 0x02) /* wake */
		}

		Case (0x10)
		{
			Printf ("EC: Wake reason: Battery low")
			Notify (\_SB.BAT0, 0x02) /* wake */
		}

		Default
		{
			Printf ("EC: Wake reason: other")
			Notify (\_SB.PWRB, 0x02) /* wake */
		}
	}
}

Method (_Q1B)
{
	Printf ("EC: _Q1B: Lid state change, state=%o", ToDecimalString (LSTE))

	Notify (\_SB.LID, 0x80)
}

Method (_Q1D)
{
	Printf ("EC: _Q1D: Power button")

	Notify (\_SB.PWRB, 0x80)
}

Method (_Q1E)
{
	Printf ("EC: _Q1E: Battery low")
}

Method (_Q32)
{
	Printf ("EC: _Q32: Battery thermal trip")
}

Method (_Q35)
{
	Printf ("EC: _Q35: Silent fan mode change, state=%o", ToDecimalString (SLFG))
}

Method (_Q37)
{
	Printf ("EC: _Q37: B15C flag change, B15C=%o", ToHexString (B15C))
}

Method (_Q42)
{
	Printf ("EC: _Q42, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q46)
{
	Printf ("EC: _Q46, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q4A)
{
	Printf ("EC: _Q4A: KBC beep on/off, SCIE=0x%o, state=%o",
		ToHexString (SCIE), ToDecimalString(SCIE & 1))
}

Method (_Q50)
{
	Printf ("EC: _Q50: SCI event, SCIE=0x%o", ToHexString (SCIE))

	Switch (ToInteger (SCIE))
	{
		Case (0x68) // L140MU only
		{
		}

		Case (0x69) // L140MU only
		{
		}

		Case (0x6a)
		{
			Printf ("EC: Fan mode: MaxQ")
		}

		Case (0x6c)
		{
			Printf ("EC: Fan mode: custom")
		}

		Case (0x7a)
		{
		}

		Case (0x7b)
		{
			Printf ("EC: Fn + Backspace pressed")
		}

		Case (0x7c)
		{
			Printf ("EC: Screen rotate (Fn + R)")
		}

		Case (0x80)
		{
			Printf ("EC: Color keyboard color change")
		}

		Case (0x81)
		{
			Printf ("EC: Color keyboard brightness down")
		}

		Case (0x82)
		{
			Printf ("EC: Color keyboard brightness up")
		}

		Case (0x8a)
		{
			Printf ("EC: White keyboard backlight toggle")
		}

		Case (0x9f)
		{
			Printf ("EC: Color keyboard backlight toggle")
		}

		Case (0xa0)
		{
		}

		Case (0xa8)
		{
			Printf ("EC: Fn + ESC pressed")
		}

		Case (0xae)
		{
			Printf ("EC: airplane mode LED off")
		}

		Case (0xaf)
		{
			Printf ("EC: airplane mode LED on")
		}

		Case (0xb0)
		{
		}

		Case (0xc7)
		{
			Printf ("EC: NumLock off")
		}

		Case (0xc8)
		{
			Printf ("EC: NumLock on")
		}

		Case (0xc9)
		{
			Printf ("EC: CapsLock off")
		}

		Case (0xca)
		{
			Printf ("EC: CapsLock on")
		}

		Case (0xcf)
		{
			Printf ("EC: ScrollLock off")
		}

		Case (0xd0)
		{
			Printf ("EC: ScrollLock on")
		}

		Case (0xf0)
		{
		}

		Case (0xf1)
		{
		}

		Case (0xf2)
		{
			Printf ("EC: Fan mode: auto")
		}

		Case (0xf3)
		{
			Printf ("EC: Fan mode: turbo")
		}
	}
}

Method (_Q51)
{
	Printf ("EC: _Q51, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q52)
{
	Printf ("EC: _Q52, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q53)
{
	Printf ("EC: _Q53, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q56)
{
	Printf ("EC: _Q56, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q57)
{
	Printf ("EC: _Q57, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q58)
{
	Printf ("EC: _Q58, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q59)
{
	Printf ("EC: _Q59, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q5A)
{
	Printf ("EC: _Q5A, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q5D)
{
	Printf ("EC: _Q5D: Performance profile hotkey pressed (Fn + 3)")
}

Method (_Q5E)
{
	Printf ("EC: _Q5E, SCIE=0x%o", ToHexString (SCIE))
}

Method (_Q61)
{
	Printf ("EC: _Q61: Board thermal trip")
}

Method (_Q62)
{
	Printf ("EC: _Q62: UCSI event")
}
