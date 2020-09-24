/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Wrapper method that calls ALIB function 1 to report current AC/DC state.
 */
Method (WAL1)
{
	/* Send ALIB Function 1 the AC/DC state */
	Local0 = Buffer (0x03) {}
	CreateWordField (Local0, 0, F1SZ)
	CreateByteField (Local0, 2, F1DA)

	/* First argument is size i.e. 3 bytes */
	F1SZ = 3

	/*
	 * Second argument is power state i.e. AC or DC.
	 * ALIB expects AC = 0, DC = 1.
	 * PWRS reports AC = 1, DC = 0.
	 *
	 * Hence, need to invert the state of PWRS.
	 */
	F1DA = \PWRS ^ 1

	Printf ("ALIB call: func 1 params %o", Local0)
	\_SB.ALIB (1, Local0)
}

Method (PNOT)
{
	/* Report AC/DC state to ALIB using WAL1() */
	\WAL1 ()
}

/*
 * Processor Object
 */
/* These devices are created at runtime */
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
