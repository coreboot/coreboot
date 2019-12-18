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

Scope (\_SB.PCI0.MCHC)
{
	Mutex (CTCM, 1)		/* CTDP Switch Mutex (sync level 1) */
	Name (CTCC, 0)		/* CTDP Current Selection */
	Name (CTCN, 0)		/* CTDP Nominal Select */
	Name (CTCD, 1)		/* CTDP Down Select */
	Name (CTCU, 2)		/* CTDP Up Select */
	Name (SPL1, 0)		/* Saved PL1 value */

	OperationRegion (MCHB, SystemMemory,
			Add (MCH_BASE_ADDRESS, 0x5000), 0x1000)
	Field (MCHB, DWordAcc, Lock, Preserve)
	{
		Offset (0x930), /* PACKAGE_POWER_SKU */
		CTDN, 15,	/* CTDP Nominal PL1 */
		Offset (0x938), /* PACKAGE_POWER_SKU_UNIT */
		PUNI, 4,	/* Power Units */
		,     4,
		EUNI, 5,	/* Energy Units */
		,     3,
		TUNI, 4,	/* Time Units */
		Offset (0x958), /* PLATFORM_INFO */
		,     40,
		LFM_, 8,	/* Maximum Efficiency Ratio (LFM) */
		Offset (0x9a0), /* TURBO_POWER_LIMIT1 */
		PL1V, 15,	/* Power Limit 1 Value */
		PL1E, 1,	/* Power Limit 1 Enable */
		PL1C, 1,	/* Power Limit 1 Clamp */
		PL1T, 7,	/* Power Limit 1 Time */
		Offset (0x9a4), /* TURBO_POWER_LIMIT2 */
		PL2V, 15,	/* Power Limit 2 Value */
		PL2E, 1,	/* Power Limit 2 Enable */
		PL2C, 1,	/* Power Limit 2 Clamp */
		PL2T, 7,	/* Power Limit 2 Time */
		Offset (0xf3c), /* CONFIG_TDP_NOMINAL */
		TARN, 8,	/* CTDP Nominal Turbo Activation Ratio */
		Offset (0xf40), /* CONFIG_TDP_LEVEL1 */
		CTDD, 15,	/* CTDP Down PL1 */
		,     1,
		TARD, 8,	/* CTDP Down Turbo Activation Ratio */
		Offset (0xf48), /* MSR_CONFIG_TDP_LEVEL2 */
		CTDU, 15,	/* CTDP Up PL1 */
		,     1,
		TARU, 8,	/* CTDP Up Turbo Activation Ratio */
		Offset (0xf50), /* CONFIG_TDP_CONTROL */
		CTCS, 2,	/* CTDP Select */
		Offset (0xf54), /* TURBO_ACTIVATION_RATIO */
		TARS, 8,	/* Turbo Activation Ratio Select */
	}

	/*
	 * Search CPU0 _PSS looking for control=arg0 and then
	 * return previous P-state entry number for new _PPC
	 *
	 * Format of _PSS:
	 *   Name (_PSS, Package () {
	 *     Package (6) { freq, power, tlat, blat, control, status }
	 *   }
	 */
	External (\_SB.CP00._PSS)
	Method (PSSS, 1, NotSerialized)
	{
		Store (One, Local0) /* Start at P1 */
		Store (SizeOf (\_SB.CP00._PSS), Local1)

		While (LLess (Local0, Local1)) {
			/* Store _PSS entry Control value to Local2 */
			ShiftRight (DeRefOf (Index (DeRefOf (Index
			      (\_SB.CP00._PSS, Local0)), 4)), 8, Local2)
			If (LEqual (Local2, Arg0)) {
				Return (Subtract (Local0, 1))
			}
			Increment (Local0)
		}

		Return (0)
	}

	/* Calculate PL2 based on chip type */
	Method (CPL2, 1, NotSerialized)
	{
		/* Haswell ULT PL2 = 25W */
		/* FIXME: update for broadwell */
		Return (Multiply (25, 8))
	}

	/* Set Config TDP Down */
	Method (STND, 0, Serialized)
	{
		If (Acquire (CTCM, 100)) {
			Return (0)
		}
		If (LEqual (CTCD, CTCC)) {
			Release (CTCM)
			Return (0)
		}

		Store ("Set TDP Down", Debug)

		/* Set CTC */
		Store (CTCD, CTCS)

		/* Set TAR */
		Store (TARD, TARS)

		/* Set PPC limit and notify OS */
		Store (PSSS (TARD), PPCM)
		PPCN ()

		/* Set PL2 */
		Store (CPL2 (CTDD), PL2V)

		/* Set PL1 */
		Store (CTDD, PL1V)

		/* Store the new TDP Down setting */
		Store (CTCD, CTCC)

		Release (CTCM)
		Return (1)
	}

	/* Set Config TDP Nominal from Down */
	Method (STDN, 0, Serialized)
	{
		If (Acquire (CTCM, 100)) {
			Return (0)
		}
		If (LEqual (CTCN, CTCC)) {
			Release (CTCM)
			Return (0)
		}

		Store ("Set TDP Nominal", Debug)

		/* Set PL1 */
		Store (CTDN, PL1V)

		/* Set PL2 */
		Store (CPL2 (CTDN), PL2V)

		/* Set PPC limit and notify OS */
		Store (PSSS (TARN), PPCM)
		PPCN ()

		/* Set TAR */
		Store (TARN, TARS)

		/* Set CTC */
		Store (CTCN, CTCS)

		/* Store the new TDP Nominal setting */
		Store (CTCN, CTCC)

		Release (CTCM)
		Return (1)
	}

	/* Calculate PL1 value based on requested TDP */
	Method (TDPP, 1, NotSerialized)
	{
		Return (Multiply (ShiftLeft (Subtract (PUNI, 1), 2), Arg0))
	}

	/* Enable Controllable TDP to limit PL1 to requested value */
	Method (CTLE, 1, Serialized)
	{
		If (Acquire (CTCM, 100)) {
			Return (0)
		}

		Store ("Enable PL1 Limit", Debug)

		/* Set _PPC to LFM */
		Store (PSSS (LFM_), Local0)
		Add (Local0, 1, PPCM)
		\PPCN ()

		/* Set TAR to LFM-1 */
		Subtract (LFM_, 1, TARS)

		/* Set PL1 to desired value */
		Store (PL1V, SPL1)
		Store (TDPP (Arg0), PL1V)

		/* Set PL1 CLAMP bit */
		Store (One, PL1C)

		Release (CTCM)
		Return (1)
	}

	/* Disable Controllable TDP */
	Method (CTLD, 0, Serialized)
	{
		If (Acquire (CTCM, 100)) {
			Return (0)
		}

		Store ("Disable PL1 Limit", Debug)

		/* Clear PL1 CLAMP bit */
		Store (Zero, PL1C)

		/* Set PL1 to normal value */
		Store (SPL1, PL1V)

		/* Set TAR to 0 */
		Store (Zero, TARS)

		/* Set _PPC to 0 */
		Store (Zero, PPCM)
		\PPCN ()

		Release (CTCM)
		Return (1)
	}
}
