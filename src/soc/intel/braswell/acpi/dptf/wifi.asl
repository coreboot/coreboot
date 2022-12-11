/* SPDX-License-Identifier: GPL-2.0-only */

Device (WIFI)
{
	Name (_HID, "INT3408")
	Name (_UID, 0)
	Name (PTYP, 0x07)
	Name (_STR, Unicode("WIFI wireless device"))

	Method (_STA)
	{
		If (\DPTE == One) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}
}
