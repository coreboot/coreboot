/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

/* XHCI Controller 0:15.0 */
Device (XHC1) {
	Name (_ADR, 0x00150000)  /* Device 21, Function 0 */

	Name (_S3D, 3)  /* D3 supported in S3 */
	Name (_S0W, 3)  /* D3 can wake device in S0 */
	Name (_S3W, 3)  /* D3 can wake system from S3 */

	/* Declare XHCI GPE status and enable bits are bit 13 */
	Name (_PRW, Package() { GPE0A_XHCI_PME_STS, 3 })

	Method (_STA, 0)
	{
		Return (0xF)
	}

	Device (RHUB)
	{
		/* Root Hub */
		Name (_ADR, Zero)

		/* USB2 */
		Device (HS01) { Name (_ADR, 1) }
		Device (HS02) { Name (_ADR, 2) }
		Device (HS03) { Name (_ADR, 3) }
		Device (HS04) { Name (_ADR, 4) }
		Device (HS05) { Name (_ADR, 5) }
		Device (HS06) { Name (_ADR, 6) }
		Device (HS07) { Name (_ADR, 7) }
		Device (HS08) { Name (_ADR, 8) }

		/* USB3 */
		Device (SS01) { Name (_ADR, 9) }
		Device (SS02) { Name (_ADR, 10) }
		Device (SS03) { Name (_ADR, 11) }
		Device (SS04) { Name (_ADR, 12) }
		Device (SS05) { Name (_ADR, 13) }
		Device (SS06) { Name (_ADR, 14) }
	}
}
