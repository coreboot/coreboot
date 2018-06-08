/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) Intel Corporation.
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

/* CNVi Controller 0:C.0 */
Device (CNVI) {
	 Name(_ADR, 0x000C0000)

	Name (_S3D, 3)  /* D3 supported in S3 */
	Name (_S0W, 3)  /* D3 can wake device in S0 */
	Name (_S3W, 3)  /* D3 can wake system from S3 */

	Name (_PRW, Package() { GPE0A_CNVI_PME_STS, 3 })

	Method (_STA, 0)
	{
		Return (0xF)
	}
}
