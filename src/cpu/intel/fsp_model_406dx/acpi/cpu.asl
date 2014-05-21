/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/* These devices are created at runtime */
External (\_PR.CPU0, DeviceObj)
External (\_PR.CPU1, DeviceObj)
External (\_PR.CPU2, DeviceObj)
External (\_PR.CPU3, DeviceObj)
External (\_PR.CPU4, DeviceObj)
External (\_PR.CPU5, DeviceObj)
External (\_PR.CPU6, DeviceObj)
External (\_PR.CPU7, DeviceObj)

/* Notify OS to re-read CPU tables, assuming ^2 CPU count */
Method (PNOT)
{
	If (LGreaterEqual (\PCNT, 2)) {
		Notify (\_PR.CPU0, 0x81)  // _CST
		Notify (\_PR.CPU1, 0x81)  // _CST
	}
	If (LGreaterEqual (\PCNT, 4)) {
		Notify (\_PR.CPU2, 0x81)  // _CST
		Notify (\_PR.CPU3, 0x81)  // _CST
	}
	If (LGreaterEqual (\PCNT, 8)) {
		Notify (\_PR.CPU4, 0x81)  // _CST
		Notify (\_PR.CPU5, 0x81)  // _CST
		Notify (\_PR.CPU6, 0x81)  // _CST
		Notify (\_PR.CPU7, 0x81)  // _CST
	}
}

/* Notify OS to re-read CPU _PPC limit, assuming ^2 CPU count */
Method (PPCN)
{
	If (LGreaterEqual (\PCNT, 2)) {
		Notify (\_PR.CPU0, 0x80)  // _PPC
		Notify (\_PR.CPU1, 0x80)  // _PPC
	}
	If (LGreaterEqual (\PCNT, 4)) {
		Notify (\_PR.CPU2, 0x80)  // _PPC
		Notify (\_PR.CPU3, 0x80)  // _PPC
	}
	If (LGreaterEqual (\PCNT, 8)) {
		Notify (\_PR.CPU4, 0x80)  // _PPC
		Notify (\_PR.CPU5, 0x80)  // _PPC
		Notify (\_PR.CPU6, 0x80)  // _PPC
		Notify (\_PR.CPU7, 0x80)  // _PPC
	}
}

/* Notify OS to re-read Throttle Limit tables, assuming ^2 CPU count */
Method (TNOT)
{
	If (LGreaterEqual (\PCNT, 2)) {
		Notify (\_PR.CPU0, 0x82)  // _TPC
		Notify (\_PR.CPU1, 0x82)  // _TPC
	}
	If (LGreaterEqual (\PCNT, 4)) {
		Notify (\_PR.CPU2, 0x82)  // _TPC
		Notify (\_PR.CPU3, 0x82)  // _TPC
	}
	If (LGreaterEqual (\PCNT, 8)) {
		Notify (\_PR.CPU4, 0x82)  // _TPC
		Notify (\_PR.CPU5, 0x82)  // _TPC
		Notify (\_PR.CPU6, 0x82)  // _TPC
		Notify (\_PR.CPU7, 0x82)  // _TPC
	}
}

/* Return a package containing enabled processor entries */
Method (PPKG)
{
	If (LGreaterEqual (\PCNT, 8)) {
		Return (Package() {\_PR.CPU0, \_PR.CPU1, \_PR.CPU2, \_PR.CPU3,
				   \_PR.CPU4, \_PR.CPU5, \_PR.CPU6, \_PR.CPU7})
	} ElseIf (LGreaterEqual (\PCNT, 4)) {
		Return (Package() {\_PR.CPU0, \_PR.CPU1, \_PR.CPU2, \_PR.CPU3})
	} ElseIf (LGreaterEqual (\PCNT, 2)) {
		Return (Package() {\_PR.CPU0, \_PR.CPU1})
	} Else {
		Return (Package() {\_PR.CPU0})
	}
}
