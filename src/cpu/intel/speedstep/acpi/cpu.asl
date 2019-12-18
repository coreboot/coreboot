/*
 * This file is part of the coreboot project.
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

/* These come from the dynamically created CPU SSDT */
External (\_SB.CNOT, MethodObj)
External (\_SB_.CP00, DeviceObj)
External (\_SB_.CP00._PPC)
External (\_SB_.CP01._PPC)

Method (PNOT)
{
	If (MPEN) {
		\_SB.CNOT (0x80) // _PPC
		Sleep(100)
		\_SB.CNOT (0x81) // _CST
	} Else { // UP
		Notify (\_SB_.CP00, 0x80)
		Sleep(0x64)
		Notify(\_SB_.CP00, 0x81)
	}
}
