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

#define CAM_EN GPP_B11 /* Active low */
#define TS_PD GPP_E7

/* Method called from LPIT prior to enter s0ix state */
Method (MS0X, 1)
{
	If (Arg0) {
		/* Turn off camera power */
		\_SB.PCI0.STXS (CAM_EN)
	} Else {
		/* Turn on camera power */
		\_SB.PCI0.CTXS (CAM_EN)
	}
}

/* Method called from _PTS prior to enter sleep state */
Method (MPTS, 1)
{
	\_SB.PCI0.LPCB.EC0.PTS (Arg0)

	/* Clear touch screen pd pin to avoid leakage */
	\_SB.PCI0.CTXS (TS_PD)
}

/* Method called from _WAK prior to wakeup */
Method (MWAK, 1)
{
	\_SB.PCI0.LPCB.EC0.WAK (Arg0)
}
