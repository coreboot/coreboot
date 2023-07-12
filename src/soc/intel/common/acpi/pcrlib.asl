/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_ACPI_PCR_LIB_
#define _SOC_INTEL_ACPI_PCR_LIB_

/* Port Id lives in bits 23:16 and register offset lives in 15:0 of address. */
#define PCR_PORTID_SHIFT	16

/* Die Index */
#define PCH_P2SB	0x00
#define IOE_P2SB	0x01

/*
 * Get PCR register base for specified Die at given PID
 * Arg0 - Die Index
 * Arg1 - PCR Port ID
 */
Method (GPCR, 2, NotSerialized)
{
	if (Arg0 == PCH_P2SB) {
		Local0 = CONFIG_PCR_BASE_ADDRESS;
	} else {
		if (Arg0 == IOE_P2SB) {
			Local0 = CONFIG_IOE_PCR_BASE_ADDRESS;
		} else {
			Printf ("Invalid Die index (%o)\n", Arg0)
			Return (0)
		}
	}

	Return (Local0 + (Arg1 << PCR_PORTID_SHIFT))
}

/*
 * Read PCR register for specified Die at PID and offset
 * Arg0 - Die Index
 * Arg1 - PCR Port ID
 * Arg2 - Register Offset
 */
Method (RPCR, 3, Serialized)
{
	OperationRegion (PCRD, SystemMemory, GPCR (Arg0, Arg1) + Arg2, 4)
	Field (PCRD, DWordAcc, NoLock, Preserve)
	{
		DATA, 32
	}
	Return (DATA)
}

/*
 * Perform PCR register AND for specified Die at PID and offset
 * Arg0 - Die Index
 * Arg1 - PCR Port ID
 * Arg2 - Register Offset
 * Arg3 - Value to AND
 */
Method (APCR, 4, Serialized)
{
	OperationRegion (PCRD, SystemMemory, GPCR (Arg0, Arg1) + Arg2, 4)
	Field (PCRD, DWordAcc, NoLock, Preserve)
	{
		DATA, 32
	}
	DATA &= Arg3

	/*
	 * After every write one needs to read an innocuous register
	 * to ensure the writes are completed for certain ports. This is done
	 * for all ports so that the callers don't need the per-port knowledge
	 * for each transaction.
	 */
	RPCR (Arg0, Arg1, Arg2)
}

/*
 * Perform PCR register OR for specified Die at PID and offset
 * Arg0 - Die Index
 * Arg1 - PCR Port ID
 * Arg2 - Register Offset
 * Arg3 - Value to OR
 */
Method (OPCR, 4, Serialized)
{
	OperationRegion (PCRD, SystemMemory, GPCR (Arg0, Arg1) + Arg2, 4)
	Field (PCRD, DWordAcc, NoLock, Preserve)
	{
		DATA, 32
	}
	DATA |= Arg3

	/*
	 * After every write one needs to read an innocuous register
	 * to ensure the writes are completed for certain ports. This is done
	 * for all ports so that the callers don't need the per-port knowledge
	 * for each transaction.
	 */
	RPCR (Arg0, Arg1, Arg2)
}

#endif /* _SOC_INTEL_ACPI_PCR_LIB_ */
