/* SPDX-License-Identifier: GPL-2.0-only */

/* These come from the dynamically created CPU SSDT */
External (\_SB.CNOT, MethodObj)
External (\_SB_.CP00, DeviceObj)
External (\_SB_.CP00._PPC)
External (\_SB_.CP01._PPC)
External (\MPEN, IntObj)

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
