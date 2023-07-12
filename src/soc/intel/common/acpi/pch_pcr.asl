/* SPDX-License-Identifier: GPL-2.0-only */

#include "pcrlib.asl"

/* APIs to access P2SB inside PCH/SoC die */

/*
 * Calculate PCR register base at specified PID
 * Arg0 - PCR Port ID
 */
Method (PCRB, 1, NotSerialized)
{
	Return (GPCR(PCH_P2SB, Arg0))
}

/*
 * Read a PCR register at specified PID and offset
 * Arg0 - PCR Port ID
 * Arg1 - Register Offset
 */
Method (PCRR, 2, Serialized)
{
	Return (RPCR(PCH_P2SB, Arg0, Arg1))
}

/*
 * AND a value with PCR register at specified PID and offset
 * Arg0 - PCR Port ID
 * Arg1 - Register Offset
 * Arg2 - Value to AND
 */
Method (PCRA, 3, Serialized)
{
	APCR(PCH_P2SB, Arg0, Arg1, Arg2)
}

/*
 * OR a value with PCR register at specified PID and offset
 * Arg0 - PCR Port ID
 * Arg1 - Register Offset
 * Arg2 - Value to OR
 */
Method (PCRO, 3, Serialized)
{
	OPCR(PCH_P2SB, Arg0, Arg1, Arg2)
}
