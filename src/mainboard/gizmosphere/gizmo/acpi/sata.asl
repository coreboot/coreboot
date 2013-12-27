/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* simple name description */

/*
Scope (_SB) {
	Device(PCI0) {
		Device(SATA) {
			Name(_ADR, 0x00110000)
			#include "sata.asl"
		}
	}
}
*/

Name(STTM, Buffer(20) {
	0x78, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
	0x78, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
	0x1f, 0x00, 0x00, 0x00
})

/* Start by clearing the PhyRdyChg bits */
Method(_INI) {
	\_GPE._L1F()
}

Device(PMRY)
{
	Name(_ADR, 0)
	Method(_GTM, 0x0, NotSerialized) {
		Return(STTM)
	}
	Method(_STM, 0x3, NotSerialized) {}

	Device(PMST) {
		Name(_ADR, 0)
		Method(_STA,0) {
			if (LGreater(P0IS,0)) {
				return (0x0F) /* sata is visible */
			}
			else {
				return  (0x00) /* sata is missing */
			}
		}
	}/* end of PMST */

	Device(PSLA)
	{
		Name(_ADR, 1)
		Method(_STA,0) {
			if (LGreater(P1IS,0)) {
				return (0x0F) /* sata is visible */
			}
			else {
				return (0x00) /* sata is missing */
			}
		}
	}	/* end of PSLA */
}   /* end of PMRY */


Device(SEDY)
{
	Name(_ADR, 1)		/* IDE Scondary Channel */
	Method(_GTM, 0x0, NotSerialized) {
		Return(STTM)
	}
	Method(_STM, 0x3, NotSerialized) {}

	Device(SMST)
	{
		Name(_ADR, 0)
		Method(_STA,0) {
			if (LGreater(P2IS,0)) {
				return (0x0F) /* sata is visible */
			}
			else {
				return (0x00) /* sata is missing */
			}
		}
	} /* end of SMST */

	Device(SSLA)
	{
		Name(_ADR, 1)
		Method(_STA,0) {
			if (LGreater(P3IS,0)) {
				return (0x0F) /* sata is visible */
			}
			else {
				return (0x00) /* sata is missing */
			}
		}
	} /* end of SSLA */
}   /* end of SEDY */

/* SATA Hot Plug Support */
Scope(\_GPE) {
	Method(_L1F,0x0,NotSerialized) {
		if (\_SB.P0PR) {
			if (LGreater(\_SB.P0IS,0)) {
				sleep(32)
			}
			Notify(\_SB.PCI0.STCR.PMRY.PMST, 0x01) /* NOTIFY_DEVICE_CHECK */
			store(one, \_SB.P0PR)
		}

		if (\_SB.P1PR) {
			if (LGreater(\_SB.P1IS,0)) {
				sleep(32)
			}
			Notify(\_SB.PCI0.STCR.PMRY.PSLA, 0x01) /* NOTIFY_DEVICE_CHECK */
			store(one, \_SB.P1PR)
		}

		if (\_SB.P2PR) {
			if (LGreater(\_SB.P2IS,0)) {
				sleep(32)
			}
			Notify(\_SB.PCI0.STCR.SEDY.SMST, 0x01) /* NOTIFY_DEVICE_CHECK */
			store(one, \_SB.P2PR)
		}

		if (\_SB.P3PR) {
			if (LGreater(\_SB.P3IS,0)) {
				sleep(32)
			}
			Notify(\_SB.PCI0.STCR.SEDY.SSLA, 0x01) /* NOTIFY_DEVICE_CHECK */
			store(one, \_SB.P3PR)
		}
	}
}
