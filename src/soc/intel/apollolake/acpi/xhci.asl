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

/* XHCI Controller 0:15.0 */
Device (XHCI) {
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

#if CONFIG(SOC_INTEL_GLK)
#include "xhci_glk_ports.asl"
#else
#include "xhci_apl_ports.asl"
#endif
	}
}
