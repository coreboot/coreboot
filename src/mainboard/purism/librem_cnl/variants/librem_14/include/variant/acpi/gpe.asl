/* SPDX-License-Identifier: GPL-2.0-only */

// GPP_E15 SWI
Method (_L49, 0, Serialized) {
	Debug = Concatenate("GPE _L49: ", ToHexString(\_SB.PCI0.LPCB.EC0.WFNO))
	If (\_SB.PCI0.LPCB.EC0.ECOK) {
		If (\_SB.PCI0.LPCB.EC0.WFNO == One) {
			Notify(\_SB.LID0, 0x80)
		}
	}
}
