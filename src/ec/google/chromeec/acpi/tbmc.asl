/* SPDX-License-Identifier: GPL-2.0-only */

Device (TBMC)
{
	Name (_HID, "GOOG0006")
	Name (_UID, 1)
	Name (_DDN, "Tablet Motion Control")
	Method (TBMC)
	{
		If (^^RCTM == 1) {
			Return (0x1)
		} Else {
			Return (0x0)
		}
	}
	Method(_STA, 0)
	{
		Return (0xB)
	}
}
