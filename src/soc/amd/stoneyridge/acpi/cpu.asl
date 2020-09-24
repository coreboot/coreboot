/* SPDX-License-Identifier: GPL-2.0-only */

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
	If (\PCNT >= 4) {
		Return (Package ()
		{
			\_SB.P000,
			\_SB.P001,
			\_SB.P002,
			\_SB.P003
		})
	} ElseIf (\PCNT>= 2) {
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
