/* SPDX-License-Identifier: GPL-2.0-only */

Device (ECPD)
{
	Name (_HID, "GOOG0003")
	Name (_UID, 1)
	Name (_DDN, "EC PD Device")
	Method(_STA, 0)
	{
		Return (0xB)
	}
}
