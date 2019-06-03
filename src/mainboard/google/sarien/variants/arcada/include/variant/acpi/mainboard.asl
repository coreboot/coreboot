/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Intel Corp.
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
#define SSD_EN GPP_H13
#define SSD_RST GPP_H12

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

	/* Clear SSD EN adn RST pin to avoid leakage */
	If (Arg0 == 5) {
		\_SB.PCI0.CTXS (SSD_RST)
		Sleep(1)
		\_SB.PCI0.CTXS (SSD_EN)
	}
}

/* Method called from _WAK prior to wakeup */
Method (MWAK, 1)
{
	\_SB.PCI0.LPCB.EC0.WAK (Arg0)
}
