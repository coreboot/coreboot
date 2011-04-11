/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */


Scope (\_SI)
{
	Method(_SST, 1, NotSerialized)
	{
		If (LEqual (Arg0, 0)) {
			/* Indicator off */

			/* power LED off */
			\_SB.PCI0.LPCB.EC.LED(0x00)
			/* suspend LED off */
			\_SB.PCI0.LPCB.EC.LED(0x07)
		}

		If (LEqual (Arg0, 1)) {
			/* working state */

			/* power LED on */
			\_SB.PCI0.LPCB.EC.LED(0x80)
			/* suspend LED off */
			\_SB.PCI0.LPCB.EC.LED(0x07)
		}

		If (LEqual (Arg0, 2)) {
			/* waking state */

			/* power LED om */
			\_SB.PCI0.LPCB.EC.LED(0x80)
			/* suspend LED blinking */
			\_SB.PCI0.LPCB.EC.LED(0xc7)
		}

		If (LEqual (Arg0, 3)) {
			/* sleep state */

			/* power LED off */
			\_SB.PCI0.LPCB.EC.LED(0x00)
			/* suspend LED on */
			\_SB.PCI0.LPCB.EC.LED(0x87)
		}
	}
}
