/* SPDX-License-Identifier: GPL-2.0-only */

	/* PCI IRQ mapping registers, C00h-C01h. */
	OperationRegion(PRQM, SystemIO, 0x00000c00, 0x00000002)
		Field(PRQM, ByteAcc, NoLock, Preserve) {
		PRQI, 0x00000008,
		PRQD, 0x00000008,  /* Offset: 1h */
	}
	IndexField(PRQI, PRQD, ByteAcc, NoLock, Preserve) {
		PIRA, 0x00000008,	/* Index 0: INTA */
		PIRB, 0x00000008,	/* Index 1: INTB */
		PIRC, 0x00000008,	/* Index 2: INTC */
		PIRD, 0x00000008,	/* Index 3: INTD */
		PIRE, 0x00000008,	/* Index 4: INTE */
		PIRF, 0x00000008,	/* Index 5: INTF */
		PIRG, 0x00000008,	/* Index 6: INTG */
		PIRH, 0x00000008,	/* Index 7: INTH */

		Offset (0x43),
		PMMC, 0x00000008,	/* Index 0x43: eMMC */

		Offset (0x62),
		PGPI, 0x00000008,	/* Index 0x62: GPIO */

		Offset (0x70),
		PI20, 0x00000008,	/* Index 0x70: I2C0 */
		PI21, 0x00000008,	/* Index 0x71: I2C1 */
		PI22, 0x00000008,	/* Index 0x72: I2C2 */
		PI23, 0x00000008,	/* Index 0x73: I2C3 */
		PUA0, 0x00000008,	/* Index 0x74: UART0 */
		PUA1, 0x00000008,	/* Index 0x75: UART1 */
		PI24, 0x00000008,	/* Index 0x76: I2C4 */
		PI25, 0x00000008,	/* Index 0x77: I2C5 */
		PUA2, 0x00000008,	/* Index 0x78: UART2 */
		PUA3, 0x00000008,	/* Index 0x79: UART3 */

		/* IO-APIC IRQs */
		Offset (0x80),
		IORA, 0x00000008,	/* Index 0x80: INTA */
		IORB, 0x00000008,	/* Index 0x81: INTB */
		IORC, 0x00000008,	/* Index 0x82: INTC */
		IORD, 0x00000008,	/* Index 0x83: INTD */
		IORE, 0x00000008,	/* Index 0x84: INTE */
		IORF, 0x00000008,	/* Index 0x85: INTF */
		IORG, 0x00000008,	/* Index 0x86: INTG */
		IORH, 0x00000008,	/* Index 0x87: INTH */

		Offset (0xC3),
		IMMC, 0x00000008,	/* Index 0xC3: eMMC */

		Offset (0xE2),
		IGPI, 0x00000008,	/* Index 0xE2: GPIO */

		Offset (0xF0),
		II20, 0x00000008,	/* Index 0xF0: I2C0 */
		II21, 0x00000008,	/* Index 0xF1: I2C1 */
		II22, 0x00000008,	/* Index 0xF2: I2C2 */
		II23, 0x00000008,	/* Index 0xF3: I2C3 */
		IUA0, 0x00000008,	/* Index 0xF4: UART0 */
		IUA1, 0x00000008,	/* Index 0xF5: UART1 */
		II24, 0x00000008,	/* Index 0xF6: I2C4 */
		II25, 0x00000008,	/* Index 0xF7: I2C5 */
		IUA2, 0x00000008,	/* Index 0xF8: UART2 */
		IUA3, 0x00000008,	/* Index 0xF9: UART3 */
	}

	/* PCI Error control register */
	OperationRegion(PERC, SystemIO, 0x00000c14, 0x00000001)
		Field(PERC, ByteAcc, NoLock, Preserve) {
		SENS, 0x00000001,
		PENS, 0x00000001,
		SENE, 0x00000001,
		PENE, 0x00000001,
	}

	/* Client Management index/data registers */
	OperationRegion(CMT, SystemIO, 0x00000c50, 0x00000002)
		Field(CMT, ByteAcc, NoLock, Preserve) {
		CMTI,	8,
		/* Client Management Data register */
		G64E,	1,
		G64O,	1,
		G32O,	2,
		,		2,
		GPSL,	2,
	}

	/* GPM Port register */
	OperationRegion(GPT, SystemIO, 0x00000c52, 0x00000001)
		Field(GPT, ByteAcc, NoLock, Preserve) {
		GPB0,1,
		GPB1,1,
		GPB2,1,
		GPB3,1,
		GPB4,1,
		GPB5,1,
		GPB6,1,
		GPB7,1,
	}

	/* Flash ROM program enable register */
	OperationRegion(FRE, SystemIO, 0x00000c6F, 0x00000001)
		Field(FRE, ByteAcc, NoLock, Preserve) {
		,     0x00000006,
		FLRE, 0x00000001,
	}

	/* PM2 index/data registers */
	OperationRegion(PM2R, SystemIO, 0x00000Cd0, 0x00000002)
		Field(PM2R, ByteAcc, NoLock, Preserve) {
		PM2I, 0x00000008,
		PM2D, 0x00000008,
	}

	/* Power Management I/O registers, TODO:PMIO is quite different in SB800. */
	OperationRegion(PIOR, SystemIO, 0x00000Cd6, 0x00000002)
		Field(PIOR, ByteAcc, NoLock, Preserve) {
		PIOI, 0x00000008,
		PIOD, 0x00000008,
	}

	IndexField (PIOI, PIOD, ByteAcc, NoLock, Preserve) {
		Offset(0x60),		/* AcpiPm1EvgBlk */
		P1EB, 16,
		Offset(0xee),
		UPWS, 3,
	}
	OperationRegion (P1E0, SystemIO, P1EB, 0x04)
		Field (P1E0, ByteAcc, Nolock, Preserve) {
		Offset(0x02),
		, 14,
		PEWD, 1,
	}
