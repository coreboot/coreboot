/* SPDX-License-Identifier: GPL-2.0-only */

/* PCI IRQ mapping registers, C00h-C01h. */
OperationRegion(PRQM, SystemIO, 0x00000c00, 0x00000002)
	Field(PRQM, ByteAcc, NoLock, Preserve) {
	PRQI, 0x00000008,
	PRQD, 0x00000008,  /* Offset: 1h */
}

/*
 * All PIC indexes are prefixed with P.
 * All IO-APIC indexes are prefixed with I.
 */
IndexField(PRQI, PRQD, ByteAcc, NoLock, Preserve) {
	PIRA, 0x00000008,	/* Index 0: INTA */
	PIRB, 0x00000008,	/* Index 1: INTB */
	PIRC, 0x00000008,	/* Index 2: INTC */
	PIRD, 0x00000008,	/* Index 3: INTD */
	PIRE, 0x00000008,	/* Index 4: INTE */
	PIRF, 0x00000008,	/* Index 5: INTF */
	PIRG, 0x00000008,	/* Index 6: INTG */
	PIRH, 0x00000008,	/* Index 7: INTH */

	Offset (0xc),
	SIRA, 0x00000008,	/* Index 0xc: Serial IRQ A */
	SIRB, 0x00000008,	/* Index 0xd: Serial IRQ B */
	SIRC, 0x00000008,	/* Index 0xe: Serial IRQ C */
	SIRD, 0x00000008,	/* Index 0xf: Serial IRQ D */
	PSCI, 0x00000008,	/* Index 0x10: SCI */
	PSB0, 0x00000008,	/* Index 0x11: SMBUS0 */
	PASF, 0x00000008,	/* Index 0x12: ASF */
	PHDA, 0x00000008,	/* Index 0x12: HDA */

	Offset (0x16),
	PPMN, 0x00000008,	/* Index 0x16: PerMon */
	PSDC, 0x00000008,	/* Index 0x17: SD */

	Offset (0x1a),
	PSIO, 0x00000008,	/* Index 0x1A: SDIO */

	Offset (0x30),
	PUS1, 0x00000008,	/* Index 0x30: USB EMU */
	Offset (0x34),
	PUS3, 0x00000008,	/* Index 0x34: XHCI0 */
	Offset (0x41),
	PSAT, 0x00000008,	/* Index 0x41: SATA */

	Offset (0x50),
	PGP0, 0x00000008,	/* Index 0x50: GPP0 */
	PGP1, 0x00000008,	/* Index 0x51: GPP1 */
	PGP2, 0x00000008,	/* Index 0x52: GPP2 */
	PGP3, 0x00000008,	/* Index 0x53: GPP3 */

	Offset (0x62),
	PGPI, 0x00000008,	/* Index 0x62: GPIO */

	Offset (0x70),
	PI20, 0x00000008,	/* Index 0x70: I2C0/I3C0 */
	PI21, 0x00000008,	/* Index 0x71: I2C1/I3C1 */
	PI22, 0x00000008,	/* Index 0x72: I2C2/I3C2 */
	PI23, 0x00000008,	/* Index 0x73: I2C3/I3C3 */
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

	Offset (0x8c),
	ISIA, 0x00000008,	/* Index 0x8c: Serial IRQ A */
	ISIB, 0x00000008,	/* Index 0x8d: Serial IRQ B */
	ISIC, 0x00000008,	/* Index 0x8e: Serial IRQ C */
	ISID, 0x00000008,	/* Index 0x8f: Serial IRQ D */
	ISCI, 0x00000008,	/* Index 0x90: SCI */
	ISB0, 0x00000008,	/* Index 0x91: SMBUS0 */
	IASF, 0x00000008,	/* Index 0x92: ASF */
	IHDA, 0x00000008,	/* Index 0x93: HDA */

	Offset (0x96),
	IPMN, 0x00000008,	/* Index 0x96: PerMon */
	ISDC, 0x00000008,	/* Index 0x97: SD */

	Offset (0x9a),
	ISIO, 0x00000008,	/* Index 0x9A: SDIO */

	Offset (0xb0),
	IUS1, 0x00000008,	/* Index 0xb0: USB EMU */
	Offset (0xb4),
	IUS3, 0x00000008,	/* Index 0xb4: XHCI0 */
	Offset (0xc1),
	ISAT, 0x00000008,	/* Index 0xc1: SATA */

	Offset (0xd0),
	IGP0, 0x00000008,	/* Index 0xD0: GPP0 */
	IGP1, 0x00000008,	/* Index 0xD1: GPP1 */
	IGP2, 0x00000008,	/* Index 0xD2: GPP2 */
	IGP3, 0x00000008,	/* Index 0xD3: GPP3 */

	Offset (0xe2),
	IGPI, 0x00000008,	/* Index 0xE2: GPIO */

	Offset (0xf0),
	II20, 0x00000008,	/* Index 0xF0: I2C0/I3C0 */
	II21, 0x00000008,	/* Index 0xF1: I2C1/I3C1 */
	II22, 0x00000008,	/* Index 0xF2: I2C2/I3C2 */
	II23, 0x00000008,	/* Index 0xF3: I2C3/I3C3 */
	IUA0, 0x00000008,	/* Index 0xF4: UART0 */
	IUA1, 0x00000008,	/* Index 0xF5: UART1 */
	II24, 0x00000008,	/* Index 0xF6: I2C4 */
	II25, 0x00000008,	/* Index 0xF7: I2C5 */
	IUA2, 0x00000008,	/* Index 0xF8: UART2 */
	IUA3, 0x00000008,	/* Index 0xF9: UART3 */
}
