/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.MCHC)
{
	Mutex (CTCM, 1)		/* CTDP Switch Mutex (sync level 1) */
	Name (CTCC, 0)		/* CTDP Current Selection */
	Name (CTCN, 0)		/* CTDP Nominal Select */
	Name (CTCD, 1)		/* CTDP Down Select */
	Name (CTCU, 2)		/* CTDP Up Select */
	Name (SPL1, 0)		/* Saved PL1 value */

	OperationRegion (MCHB, SystemMemory, CONFIG_FIXED_MCHBAR_MMIO_BASE + 0x5000, 0x1000)
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
	 * Search CPU0 _PSS looking for control = arg0 and then
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
		Local0 = 1 /* Start at P1 */
		Local1 = SizeOf (\_SB.CP00._PSS)

		While (Local0 < Local1) {
			/* Store _PSS entry Control value to Local2 */
			Local2 = DeRefOf (DeRefOf (\_SB.CP00._PSS[Local0])[4]) >> 8
			If (Local2 == Arg0) {
				Return (Local0 - 1)
			}
			Local0++
		}

		Return (0)
	}

	/* Calculate PL2 based on chip type */
	Method (CPL2, 1, NotSerialized)
	{
#if CONFIG(INTEL_LYNXPOINT_LP)
		/* Haswell ULT PL2 = 25W */
		Return (25 * 8)
#else
		/* Haswell Mobile PL2 = 1.25 * PL1 */
		Return ((Arg0 * 125) / 100)
#endif
	}

	/* Set Config TDP Down */
	Method (STND, 0, Serialized)
	{
		If (Acquire (CTCM, 100)) {
			Return (0)
		}
		If (CTCD == CTCC) {
			Release (CTCM)
			Return (0)
		}

		Printf ("Set TDP Down")

		/* Set CTC */
		CTCS = CTCD

		/* Set TAR */
		TARS = TARD

		/* Set PPC limit and notify OS */
		PPCM = PSSS (TARD)
		PPCN ()

		/* Set PL2 */
		PL2V = CPL2 (CTDD)

		/* Set PL1 */
		PL1V = CTDD

		/* Store the new TDP Down setting */
		CTCC = CTCD

		Release (CTCM)
		Return (1)
	}

	/* Set Config TDP Nominal from Down */
	Method (STDN, 0, Serialized)
	{
		If (Acquire (CTCM, 100)) {
			Return (0)
		}
		If (CTCN == CTCC) {
			Release (CTCM)
			Return (0)
		}

		Printf ("Set TDP Nominal")

		/* Set PL1 */
		PL1V = CTDN

		/* Set PL2 */
		PL2V = CPL2 (CTDN)

		/* Set PPC limit and notify OS */
		PPCM = PSSS (TARN)
		PPCN ()

		/* Set TAR */
		TARS = TARN

		/* Set CTC */
		CTCS = CTCN

		/* Store the new TDP Nominal setting */
		CTCC = CTCN

		Release (CTCM)
		Return (1)
	}

	/* Calculate PL1 value based on requested TDP */
	Method (TDPP, 1, NotSerialized)
	{
		Return (((PUNI - 1) << 2) * Arg0)
	}

	/* Enable Controllable TDP to limit PL1 to requested value */
	Method (CTLE, 1, Serialized)
	{
		If (Acquire (CTCM, 100)) {
			Return (0)
		}

		Printf ("Enable PL1 Limit")

		/* Set _PPC to LFM */
		Local0 = PSSS (LFM_)
		PPCM = Local0 + 1
		\PPCN ()

		/* Set TAR to LFM-1 */
		TARS = LFM_ - 1

		/* Set PL1 to desired value */
		SPL1 = PL1V
		PL1V = TDPP (Arg0)

		/* Set PL1 CLAMP bit */
		PL1C = 1

		Release (CTCM)
		Return (1)
	}

	/* Disable Controllable TDP */
	Method (CTLD, 0, Serialized)
	{
		If (Acquire (CTCM, 100)) {
			Return (0)
		}

		Printf ("Disable PL1 Limit")

		/* Clear PL1 CLAMP bit */
		PL1C = 0

		/* Set PL1 to normal value */
		PL1V = SPL1

		/* Set TAR to 0 */
		TARS = 0

		/* Set _PPC to 0 */
		PPCM = 0
		\PPCN ()

		Release (CTCM)
		Return (1)
	}
}
