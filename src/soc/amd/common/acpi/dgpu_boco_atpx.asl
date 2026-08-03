/* SPDX-License-Identifier: GPL-2.0-only */

/* PCIe switch Upstream Device */
Device (SWUS)
{
	Name (_ADR, 0)
	Name (_S0W, 0x04)	/* ACPI_DEVICE_SLEEP_D3_COLD */

	/* PCIe switch Downstream Device */
	Device (SWDS) {
		Name (_ADR, 0)

		/* Graphics card behind Downstream device */
		Device (DGPU) {
			Name (_ADR, 0)
			/* Implemented at board level: Set power state */
			External(BOCO, MethodObj)
			/* Implemented at board level: Get power state */
			External(PWRS, MethodObj)
			/* Implemented at board level: Get BOCO enabled state */
			External(BCEN, MethodObj)
		}
	}

	OperationRegion (PCFG, PCI_Config, Zero, 0x0100)
	Field (PCFG, AnyAcc, NoLock, Preserve)
	{
		Offset (0x00),
		PC00, 16,	/* VENDOR ID */
		PC02, 16,	/* DEVICE ID */
		PC04, 16,	/* COMMAND */
		Offset (0x18),
		PC18, 8,	/* Primary Bus */
		PC19, 8,	/* Secondary Bus */
		PC1A, 8,	/* Subordinate Bus */
		PC1B, 8,	/* Sec. Latency Timer */
		Offset (0x20),
		PC20, 32,	/* Memory Base/Limit */
		PC24, 32,	/* Prefetchable Mem */
		PC28, 32,	/* Prefetchable Base Upper */
		PC2C, 32	/* Prefetchable Base Lower */
	}

	Name (SV04, Zero)
	Name (SV18, Zero)
	Name (SV19, Zero)
	Name (SV1A, Zero)
	Name (SV1B, Zero)
	Name (SV20, Zero)
	Name (SV24, Zero)
	Name (SV28, Zero)
	Name (SV2C, Zero)

	Method (SAVE, 0, Serialized)
	{
		SV04 = PC04
		SV18 = PC18
		SV19 = PC19
		SV1A = PC1A
		SV1B = PC1B
		SV20 = PC20
		SV24 = PC24
		SV28 = PC28
		SV2C = PC2C
	}

	Method (RSTR, 0, Serialized)
	{
		PC18 = SV18
		PC19 = SV19
		PC1A = SV1A
		PC1B = SV1B
		PC20 = SV20
		PC24 = SV24
		PC28 = SV28
		PC2C = SV2C
		/* Restore PCI_COMMAND last */
		PC04 = SV04
	}

	Name (_PR0, Package (0x01){PRIC})
	Name (_PR2, Package (0x01){PRIC})
	Name (_PR3, Package (0x01){PRIC})

	/* Dummy PowerResource */
	PowerResource (PRIC, 0, 0)
	{
		Name (FLAG, One)
		Method (_STA, 0, NotSerialized) {Return (FLAG)}
		Method (_ON, 0, NotSerialized) {FLAG = One}
		Method (_OFF, 0, NotSerialized) {FLAG = Zero}
	}
}

Scope (SWUS.SWDS)
{
	Name (_S0W, 0x04)	/* ACPI_DEVICE_SLEEP_D3_COLD */

	OperationRegion (PCFG, PCI_Config, Zero, 0x0100)
	Field (PCFG, AnyAcc, NoLock, Preserve)
	{
		Offset (0x00),
		PC00, 16,	/* VENDOR ID */
		PC02, 16,	/* DEVICE ID */
		PC04, 16,	/* COMMAND */
		Offset (0x18),
		PC18, 8,	/* Primary Bus */
		PC19, 8,	/* Secondary Bus */
		PC1A, 8,	/* Subordinate Bus */
		PC1B, 8,	/* Sec. Latency Timer */
		Offset (0x20),
		PC20, 32,	/* Memory Base/Limit */
		PC24, 32,	/* Prefetchable Mem */
		PC28, 32,	/* Prefetchable Base Upper */
		PC2C, 32	/* Prefetchable Base Lower */
	}

	Name (SV04, Zero)
	Name (SV18, Zero)
	Name (SV19, Zero)
	Name (SV1A, Zero)
	Name (SV1B, Zero)
	Name (SV20, Zero)
	Name (SV24, Zero)
	Name (SV28, Zero)
	Name (SV2C, Zero)

	Method (SAVE, 0, Serialized)
	{
		SV04 = PC04
		SV18 = PC18
		SV19 = PC19
		SV1A = PC1A
		SV1B = PC1B
		SV20 = PC20
		SV24 = PC24
		SV28 = PC28
		SV2C = PC2C
	}

	Method (RSTR, 0, Serialized)
	{
		PC18 = SV18
		PC19 = SV19
		PC1A = SV1A
		PC1B = SV1B
		PC20 = SV20
		PC24 = SV24
		PC28 = SV28
		PC2C = SV2C
		/* Restore PCI_COMMAND last */
		PC04 = SV04
	}

	Name (_PR0, Package (0x01){PRIC})
	Name (_PR2, Package (0x01){PRIC})
	Name (_PR3, Package (0x01){PRIC})

	/* Dummy PowerResource */
	PowerResource (PRIC, 0, 0)
	{
		Name (FLAG, One)
		Method (_STA, 0, NotSerialized) {Return (FLAG)}
		Method (_ON, 0, NotSerialized) {FLAG = One}
		Method (_OFF, 0, NotSerialized) {FLAG = Zero}
	}
}

Scope (SWUS.SWDS.DGPU)
{
	OperationRegion (PCFG, PCI_Config, Zero, 0x0100)
	Field (PCFG, AnyAcc, NoLock, Preserve)
	{
		Offset (0x00),
		PC00, 16,	/* VENDOR ID */
		PC02, 16,	/* DEVICE ID */
		PC04, 16,	/* COMMAND */
		Offset (0x10),
		PC10, 32,	/* BAR0 */
		PC14, 32,	/* BAR1 */
		PC18, 32,	/* BAR2 */
		PC1C, 32,	/* BAR3 */
		PC20, 32,	/* BAR4 */
		PC24, 32	/* BAR5 */
	}

	Name (SV04, Zero)
	Name (SV10, Zero)
	Name (SV14, Zero)
	Name (SV18, Zero)
	Name (SV1C, Zero)
	Name (SV20, Zero)
	Name (SV24, Zero)

	Method (SAVE, 0, Serialized)
	{
		SV04 = PC04
		SV10 = PC10
		SV14 = PC14
		SV18 = PC18
		SV1C = PC1C
		SV20 = PC20
		SV24 = PC24
	}

	Method (RSTR, 0, Serialized)
	{
		PC10 = SV10
		PC14 = SV14
		PC18 = SV18
		PC1C = SV1C
		PC20 = SV20
		PC24 = SV24
		/* Restore PCI_COMMAND last */
		PC04 = SV04
	}

	/* Create local variables for ATPX */
	Name (ATPV, Buffer (8) {
		0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0
	})

	CreateWordField (ATPV, 0x00, SIZE)
	CreateWordField (ATPV, 0x02, VERS)
	CreateDWordField (ATPV, 0x04, FUNC)

	Name (ATPP, Buffer (10) {
		0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0,
		0x0, 0x0
	})

	CreateWordField (ATPP, 0x00, PLEN)
	CreateDWordField (ATPP, 0x02, VALI)
	CreateDWordField (ATPP, 0x06, IMPL)

	/* Graphics driver use ATPX to query capabilities and control power */
	Method (ATPX, 2, Serialized)
	{
		/* Version request */
		if (Arg0 == 0x0)
		{
			/* Assemble and return version information */
			SIZE = 8	/* Response length */
			VERS = 1	/* Version number */
			FUNC = 1	/* Supported functions:
					 * Only advertise 'Get supported parameters'.
					 */

			Return (ATPV)
		}

		/* Get supported parameters */
		If (Arg0 == One)
		{
			PLEN = 10
			VALI = 0

			/* BOCO implemented in mainboard code? */
			If (CondRefOf (BOCO) && CondRefOf (PWRS) && CondRefOf (BCEN))
			{
				If (BCEN() == 1) {
					VALI |= 0x4000	/* ATPX_MS_HYBRID_GFX_SUPPORTED */
					Debug = "Boco support enabled in mainboard"
				}
			}
			IMPL = VALI
			Return (ATPP)
		}

		Return (Buffer (0x08)
		{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		})
	}
}

Mutex (MUTC, 0)

/* For BOCO (ATPX Power control methods not used) */
Method (GPWR, 1, Serialized)
{
	If (!CondRefOf(^SWUS.SWDS.DGPU.BOCO))
	{
		Return
	}

	Local0 = Acquire (MUTC, 1000)
	/* check for Mutex acquired */
	If (Local0 != 0)
	{
		Return
	}

	/* Clear slot presence detect */
	DSLP ()

	If (Arg0 == 0) {
		/* Backup registers */
		^SWUS.SAVE ()
		^SWUS.SWDS.SAVE ()
		^SWUS.SWDS.DGPU.SAVE ()

		/* Backup PCI_PM_CTRL */
		Local7 = STPM ()

		/* Set PM D0 state */
		SPMD (0)

		/* Disable ASPM */
		DASP ()

		/* Restore PCI_PM_CTRL */
		RSPM (Local7)

		/* Disable link */
		LDIS ()

		Sleep (10)

		/* Assert reset and disable power */
		^SWUS.SWDS.DGPU.BOCO (Zero)

		/* Clear slot presence detect */
		DSLP ()
	} else {
		/* Enable power and release reset */
		^SWUS.SWDS.DGPU.BOCO (One)

		/* Retrain link */
		RETR ()

		/* Backup PCI_PM_CTRL */
		Local7 = STPM ()

		/* Set PM D0 state */
		SPMD (0)

		/* Wait for downstream device to appear */
		Local0 = 500
		While (Local0 > 0) {
			If (^SWUS.PC00 != 0xffff && ^SWUS.PC00 != 0x0000)
			{
				Break
			}
			Sleep (1)
			Local0 --
		}

		/* Restore PCI_PM_CTRL */
		RSPM (Local7)

		/* Restore registers */
		^SWUS.SWDS.DGPU.RSTR ()
		^SWUS.SWDS.RSTR ()
		^SWUS.RSTR ()
	}

	Release (MUTC)
	Return
}

Name (_PR0, Package (0x01){PRIC})
Name (_PR2, Package (0x01){PRIC})
Name (_PR3, Package (0x01){PRIC})

/* PowerResource used instead of ATPX */
PowerResource (PRIC, 0, 0)
{
	Name (FLAG, Zero)

	Method (_STA, 0, NotSerialized) {
		If (CondRefOf (^^SWUS.SWDS.DGPU.PWRS))
		{
			FLAG = ^^SWUS.SWDS.DGPU.PWRS()
		}

		Return (FLAG)
	}
	Method (_ON, 0, NotSerialized) {
		If (FLAG == Zero) {
			FLAG = One
			GPWR(One)
		}
	}
	Method (_OFF, 0, NotSerialized) {
		If (FLAG == One) {
			FLAG = Zero
			GPWR(Zero)
		}
	}
}
