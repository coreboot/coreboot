/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/amd/common/acpi/alib.asl>

Method (PNOT)
{
	/* Report AC/DC state to ALIB using WAL1() */
	\WAL1 ()
}

/*
 * Processor Object
 */
/* These devices are created at runtime */
External (\PCNT, IntObj)
External (\_SB.C000, DeviceObj)
External (\_SB.C001, DeviceObj)
External (\_SB.C002, DeviceObj)
External (\_SB.C003, DeviceObj)
External (\_SB.C004, DeviceObj)
External (\_SB.C005, DeviceObj)
External (\_SB.C006, DeviceObj)
External (\_SB.C007, DeviceObj)

/* Return a package containing enabled processor entries */
Method (PPKG)
{
	If (\PCNT >= 8) {
		Return (Package ()
		{
			\_SB.C000,
			\_SB.C001,
			\_SB.C002,
			\_SB.C003,
			\_SB.C004,
			\_SB.C005,
			\_SB.C006,
			\_SB.C007
		})
	} ElseIf (\PCNT >= 4) {
		Return (Package ()
		{
			\_SB.C000,
			\_SB.C001,
			\_SB.C002,
			\_SB.C003
		})
	} ElseIf (\PCNT >= 2) {
		Return (Package ()
		{
			\_SB.C000,
			\_SB.C001
		})
	} Else {
		Return (Package ()
		{
			\_SB.C000
		})
	}
}
