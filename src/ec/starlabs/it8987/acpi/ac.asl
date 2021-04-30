/* SPDX-License-Identifier: GPL-2.0-only */

Device (ADP1)
{
	Name (_HID, "ACPI0003")
	Name (_PCL, Package () { \_SB })

	Method (_STA, 0, NotSerialized)		// _STA: Status
	{
		If (ECON == 1)
		{
			Local0 = 0x0F
		}
		Else
		{
			Local0 = 0
		}
		Return (Local0)
	}

	Method (_PSR, 0, NotSerialized)		// _PSR: Power Source
	{
		If (ECWR & 0x01)
		{
			\PWRS = 1
		}
		Else
		{
			\PWRS = 0
		}
		Return (\PWRS)
	}
}

Method (_QA0, 0, NotSerialized)			// AC Power Connected
{
	If (ECWR & 0x01)
	{
		\PWRS = 1
	}
	Else
	{
		\PWRS = 0
	}

	// 500ms delay - Not used in coreboot
	// Sleep (500)
	Notify (BAT0, 0x81)
	// Sleep (500)
	Notify (ADP1, 0x80)
}

Method(_Q0B, 0, NotSerialized)			// Battery Connected
{
	// 500ms delay - Not used in coreboot
	// Sleep (500)
	Notify (BAT0, 0x81)
	// Sleep (500)
	Notify (BAT0, 0x80)
}
