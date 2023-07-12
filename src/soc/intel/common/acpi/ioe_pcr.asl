/* SPDX-License-Identifier: GPL-2.0-only */

#include "pcrlib.asl"

/* APIs to access P2SB inside IOE die */

/*
 * Calculate PCR register base at specified PID
 * Arg0 - PCR Port ID
 */
Method (ICRB, 1, NotSerialized)
{
	Return (GPCR(IOE_P2SB, Arg0))
}

/*
 * Read a PCR register at specified PID and offset
 * Arg0 - PCR Port ID
 * Arg1 - Register Offset
 */
Method (ICRR, 2, Serialized)
{
	Return (RPCR(IOE_P2SB, Arg0, Arg1))
}

/*
 * AND a value with PCR register at specified PID and offset
 * Arg0 - PCR Port ID
 * Arg1 - Register Offset
 * Arg2 - Value to AND
 */
Method (ICRA, 3, Serialized)
{
	APCR(IOE_P2SB, Arg0, Arg1, Arg2)
}

/*
 * OR a value with PCR register at specified PID and offset
 * Arg0 - PCR Port ID
 * Arg1 - Register Offset
 * Arg2 - Value to OR
 */
Method (ICRO, 3, Serialized)
{
	OPCR(IOE_P2SB, Arg0, Arg1, Arg2)
}
