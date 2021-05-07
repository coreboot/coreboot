/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/alib.h>

/* The ALIB method object is defined in an SSDT */
External(\_SB.ALIB, MethodObj)

/* Wrapper method that calls ALIB function 1 to report current AC/DC state. */
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
	\_SB.ALIB (ALIB_FUNCTION_REPORT_AC_DC_STATE, Local0)
}
