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

/* Required function by EC, Notify OS to re-read CPU tables */
Method (PNOT)
{
}

/*
 * Processor Object
 */
/* These devices are created at runtime */
External (\_SB.P000, DeviceObj)
External (\_SB.P001, DeviceObj)
External (\_SB.P002, DeviceObj)
External (\_SB.P003, DeviceObj)
External (\_SB.P004, DeviceObj)
External (\_SB.P005, DeviceObj)
External (\_SB.P006, DeviceObj)
External (\_SB.P007, DeviceObj)

/* Return a package containing enabled processor entries */
Method (PPKG)
{
	If (LGreaterEqual (\PCNT, 2)) {
		Return (Package ()
		{
			\_SB.P000,
			\_SB.P001
		})
	} Else {
		Return (Package ()
		{
			\_SB.P000
		})
	}
}
