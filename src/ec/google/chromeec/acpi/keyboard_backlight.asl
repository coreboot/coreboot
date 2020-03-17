/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Scope (\_SB)
{
	/*
	 * Chrome EC Keyboard Backlight interface
	 */
	Device (KBLT)
	{
		Name (_HID, "GOOG0002")
		Name (_UID, 1)

		/* Ask EC if we even have a backlight
		 * Return 0xf (present, enabled, show in UI, functioning) or 0
		 *
		 * With older EC codebases that don't support the Device
		 * Features bitfield, this reports the keyboard backlight as
		 * enabled since reads to undefined addresses in EC address
		 * space return 0xff and so KBLE will be 1.
		 */
		Method (_STA, 0, NotSerialized)
		{
			/* If query is unsupported, but this code is compiled
			 * in, assume the backlight exists physically.
			 */
			If (LEqual (1, \_SB.PCI0.LPCB.EC0.DFUD)) {
				Return (0xf)
			}
			/* If EC reports that backlight exists, trust it */
			If (LEqual (1, \_SB.PCI0.LPCB.EC0.KBLE)) {
				Return (0xf)
			}
			/* Otherwise: no device -> disable */
			Return (0)
		}

		/* Read current backlight value */
		Method (KBQC, 0, NotSerialized)
		{
			Return (\_SB.PCI0.LPCB.EC0.KBLV)
		}

		/* Write new backlight value */
		Method (KBCM, 1, NotSerialized)
		{
			Store (Arg0, \_SB.PCI0.LPCB.EC0.KBLV)
		}
	}
}
