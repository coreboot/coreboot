/* SPDX-License-Identifier: GPL-2.0-only */

/* Method called from _PTS prior to enter sleep state */
Method (MPTS, 1) {
	\_SB.PCI0.LPCB.EC0.PTS (Arg0)
}

/* Method called from _WAK prior to wakeup */
Method (MWAK, 1) {
	\_SB.PCI0.LPCB.EC0.WAK (Arg0)
}
