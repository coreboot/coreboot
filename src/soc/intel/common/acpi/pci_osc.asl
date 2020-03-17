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

#define PCI_OSC_UUID "33DB4D5B-1FF7-401C-9657-7441C03DD766"

Scope (\_SB.PCI0) {
	Method (_OSC, 4) {
		/* Check for proper GUID */
		If (LEqual (Arg0, ToUUID (PCI_OSC_UUID))) {
			/* Let OS control everything */
			Return (Arg3)
		} Else {
			/* Unrecognized UUID */
			CreateDWordField (Arg3, 0, CDW1)
			Or (CDW1, 4, CDW1)
			Return (Arg3)
		}
	}
}
