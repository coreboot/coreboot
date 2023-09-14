/* SPDX-License-Identifier: GPL-2.0-only */

Scope (_GPE)
{
	/*
	 * Keyboard dock connector sense.  This is GPP_D4, which would seem to
	 * be _L34 for GPE0_DW1, but there is an additional 10h offset for some
	 * reason, even if GPP_D is assigned to DW0 or DW2 instead.
	 */
	Method (_L44, 0, NotSerialized) {
		Printf ("GPE _L44");
		\_SB.PCI0.LPCB.EC0.VBTN.NTFY ()
	}
}
