/* SPDX-License-Identifier: GPL-2.0-only */

/* Method called from _PTS prior to enter sleep state */
Method (MPTS, 1) {}

/* Method called from _WAK prior to wakeup */
Method (MWAK, 1) {
	Return(Package(){0, 0})
}
