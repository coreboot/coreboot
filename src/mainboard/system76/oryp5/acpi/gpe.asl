/* SPDX-License-Identifier: GPL-2.0-only */

// GPP_B23 SCI
Method (_L17, 0, Serialized)
{
	Debug = Concatenate("GPE _L17: ", ToHexString(\_SB.PCI0.LPCB.EC0.WFNO))
	If (\_SB.PCI0.LPCB.EC0.ECOK) {
		If (\_SB.PCI0.LPCB.EC0.WFNO == 1) {
			Notify(\_SB.LID0, 0x80)
		}
	}
}
