/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */


Name(_HID,EISAID("PNP0A08"))	// PCIe
Name(_CID,EISAID("PNP0A03"))	// PCI

Name(_BBN, 0)

Device (MCHC)
{
	Name(_ADR, 0x00000000)	// 0:0.0

	OperationRegion(MCHP, PCI_Config, 0x00, 0x100)
	Field (MCHP, DWordAcc, NoLock, Preserve)
	{
		Offset (0x40),	// EPBAR
		EPEN,	 1,	// Enable
		,	11,	//
		EPBR,	24,	// EPBAR

		Offset (0x48),	// MCHBAR
		MHEN,	 1,	// Enable
		,	13,	//
		MHBR,	22,	// MCHBAR
		Offset (0x54),
		DVEN,	32,
		Offset (0x60),	// PCIe BAR
		PXEN,	 1,	// Enable
		PXSZ,	 2,	// BAR size
		,	23,	//
		PXBR,	10,	// PCIe BAR

		Offset (0x68),	// DMIBAR
		DMEN,	 1,	// Enable
		,	11,	//
		DMBR,	24,	// DMIBAR

		Offset (0x70),	// ME Base Address
		MEBA,	 64,

		// ...

		Offset (0x80),	// PAM0
		,	 4,
		PM0H,	 2,
		,	 2,
		Offset (0x81),	// PAM1
		PM1L,	 2,
		,	 2,
		PM1H,	 2,
		,	 2,
		Offset (0x82),	// PAM2
		PM2L,	 2,
		,	 2,
		PM2H,	 2,
		,	 2,
		Offset (0x83),	// PAM3
		PM3L,	 2,
		,	 2,
		PM3H,	 2,
		,	 2,
		Offset (0x84),	// PAM4
		PM4L,	 2,
		,	 2,
		PM4H,	 2,
		,	 2,
		Offset (0x85),	// PAM5
		PM5L,	 2,
		,	 2,
		PM5H,	 2,
		,	 2,
		Offset (0x86),	// PAM6
		PM6L,	 2,
		,	 2,
		PM6H,	 2,
		,	 2,

		Offset (0xa0),	// Top of Used Memory
		TOM,	 64,

		Offset (0xbc),	// Top of Low Used Memory
		TLUD,	 32,
	}

	Mutex (CTCM, 1)		/* CTDP Switch Mutex (sync level 1) */
	Name (CTCC, 0)		/* CTDP Current Selection */
	Name (CTCN, 0)		/* CTDP Nominal Select */
	Name (CTCD, 1)		/* CTDP Down Select */
	Name (CTCU, 2)		/* CTDP Up Select */
	Name (SPL1, 0)		/* Saved PL1 value */

	OperationRegion (MCHB, SystemMemory, Add(DEFAULT_MCHBAR,0x5000), 0x1000)
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
		If (\ISLP ()) {
			/* Haswell ULT PL2 = 25W */
			Return (Multiply (25, 8))
		} Else {
			/* Haswell Mobile PL2 = 1.25 * PL1 */
			Return (Divide (Multiply (Arg0, 125), 100))
		}
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

// Current Resource Settings
Name (MCRS, ResourceTemplate()
{
	// Bus Numbers
	WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
			0x0000, 0x0000, 0x00ff, 0x0000, 0x0100,,, PB00)

	// IO Region 0
	DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
			0x0000, 0x0000, 0x0cf7, 0x0000, 0x0cf8,,, PI00)

	// PCI Config Space
	Io (Decode16, 0x0cf8, 0x0cf8, 0x0001, 0x0008)

	// IO Region 1
	DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
			0x0000, 0x0d00, 0xffff, 0x0000, 0xf300,,, PI01)

	// VGA memory (0xa0000-0xbffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000a0000, 0x000bffff, 0x00000000,
			0x00020000,,, ASEG)

	// OPROM reserved (0xc0000-0xc3fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000c0000, 0x000c3fff, 0x00000000,
			0x00004000,,, OPR0)

	// OPROM reserved (0xc4000-0xc7fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000c4000, 0x000c7fff, 0x00000000,
			0x00004000,,, OPR1)

	// OPROM reserved (0xc8000-0xcbfff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000c8000, 0x000cbfff, 0x00000000,
			0x00004000,,, OPR2)

	// OPROM reserved (0xcc000-0xcffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000cc000, 0x000cffff, 0x00000000,
			0x00004000,,, OPR3)

	// OPROM reserved (0xd0000-0xd3fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000d0000, 0x000d3fff, 0x00000000,
			0x00004000,,, OPR4)

	// OPROM reserved (0xd4000-0xd7fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000d4000, 0x000d7fff, 0x00000000,
			0x00004000,,, OPR5)

	// OPROM reserved (0xd8000-0xdbfff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000d8000, 0x000dbfff, 0x00000000,
			0x00004000,,, OPR6)

	// OPROM reserved (0xdc000-0xdffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000dc000, 0x000dffff, 0x00000000,
			0x00004000,,, OPR7)

	// BIOS Extension (0xe0000-0xe3fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000e0000, 0x000e3fff, 0x00000000,
			0x00004000,,, ESG0)

	// BIOS Extension (0xe4000-0xe7fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000e4000, 0x000e7fff, 0x00000000,
			0x00004000,,, ESG1)

	// BIOS Extension (0xe8000-0xebfff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000e8000, 0x000ebfff, 0x00000000,
			0x00004000,,, ESG2)

	// BIOS Extension (0xec000-0xeffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000ec000, 0x000effff, 0x00000000,
			0x00004000,,, ESG3)

	// System BIOS (0xf0000-0xfffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000f0000, 0x000fffff, 0x00000000,
			0x00010000,,, FSEG)

	// PCI Memory Region (Top of memory-CONFIG_MMCONF_BASE_ADDRESS)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000,,, PM01)

	// TPM Area (0xfed40000-0xfed44fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0xfed40000, 0xfed44fff, 0x00000000,
			0x00005000,,, TPMR)
})

Method (_CRS, 0, Serialized)
{
	// Find PCI resource area in MCRS
	CreateDwordField(MCRS, ^PM01._MIN, PMIN)
	CreateDwordField(MCRS, ^PM01._MAX, PMAX)
	CreateDwordField(MCRS, ^PM01._LEN, PLEN)

	// Fix up PCI memory region
	// Start with Top of Lower Usable DRAM
	Store (^MCHC.TLUD, Local0)
	Store (^MCHC.MEBA, Local1)

	// Check if ME base is equal
	If (LEqual (Local0, Local1)) {
		// Use Top Of Memory instead
		Store (^MCHC.TOM, Local0)
	}

	Store (Local0, PMIN)
	Store (Subtract(CONFIG_MMCONF_BASE_ADDRESS, 1), PMAX)
	Add(Subtract(PMAX, PMIN), 1, PLEN)

	Return (MCRS)
}
