/* SPDX-License-Identifier: GPL-2.0-only */

Device (WWAN)
{
	Name (_HID, "INT3408")
	Name (_UID, 0)
	Name (PTYP, 0xF)
	Name (_STR, Unicode("Wireless Wide Area Network"))

	Method (_STA)
	{
		If (\DPTE == 1) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}
}
