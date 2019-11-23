/* SPDX-License-Identifier: GPL-2.0-only */

/* Required function by EC, Notify OS to re-read CPU tables */
Method (PNOT)
{
}

/*
 * Processor Object
 */
/* These devices are created at runtime */
External (\_PR.C000, DeviceObj)
External (\_PR.C001, DeviceObj)
External (\_PR.C002, DeviceObj)
External (\_PR.C003, DeviceObj)
External (\_PR.C004, DeviceObj)
External (\_PR.C005, DeviceObj)
External (\_PR.C006, DeviceObj)
External (\_PR.C007, DeviceObj)

/* Return a package containing enabled processor entries */
Method (PPKG)
{
	If (LGreaterEqual (\PCNT, 8)) {
		Return (Package ()
		{
			\_PR.C000,
			\_PR.C001,
			\_PR.C002,
			\_PR.C003,
			\_PR.C004,
			\_PR.C005,
			\_PR.C006,
			\_PR.C007
		})
	} ElseIf (LGreaterEqual (\PCNT, 4)) {
		Return (Package ()
		{
			\_PR.C000,
			\_PR.C001,
			\_PR.C002,
			\_PR.C003
		})
	} ElseIf (LGreaterEqual (\PCNT, 2)) {
		Return (Package ()
		{
			\_PR.C000,
			\_PR.C001
		})
	} Else {
		Return (Package ()
		{
			\_PR.C000
		})
	}
}
