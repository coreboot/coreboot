/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

/*
 * Calculate PCR register base at specified PID
 * Arg0 - PCR Port ID
 */
Method (PCRB, 1, NotSerialized)
{
	Return (Add (PCH_PCR_BASE_ADDRESS, ShiftLeft (Arg0, PCR_PORTID_SHIFT)))
}

/*
 * Read a PCR register at specified PID and offset
 * Arg0 - PCR Port ID
 * Arg1 - Register Offset
 */
Method (PCRR, 2, Serialized)
{
	OperationRegion (PCRD, SystemMemory, Add (PCRB (Arg0), Arg1), 4)
	Field (PCRD, DWordAcc, NoLock, Preserve)
	{
		DATA, 32
	}
	Return (DATA)
}

/*
 * AND a value with PCR register at specified PID and offset
 * Arg0 - PCR Port ID
 * Arg1 - Register Offset
 * Arg2 - Value to AND
 */
Method (PCRA, 3, Serialized)
{
	OperationRegion (PCRD, SystemMemory, Add (PCRB (Arg0), Arg1), 4)
	Field (PCRD, DWordAcc, NoLock, Preserve)
	{
		DATA, 32
	}
	And (DATA, Arg2, DATA)
}

/*
 * OR a value with PCR register at specified PID and offset
 * Arg0 - PCR Port ID
 * Arg1 - Register Offset
 * Arg2 - Value to OR
 */
Method (PCRO, 3, Serialized)
{
	OperationRegion (PCRD, SystemMemory, Add (PCRB (Arg0), Arg1), 4)
	Field (PCRD, DWordAcc, NoLock, Preserve)
	{
		DATA, 32
	}
	Or (DATA, Arg2, DATA)
}
