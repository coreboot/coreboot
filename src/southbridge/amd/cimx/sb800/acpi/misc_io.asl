/* SPDX-License-Identifier: GPL-2.0-only */


/* Client Management index/data registers */
OperationRegion(CMT, SystemIO, 0x00000C50, 0x00000002)
	Field(CMT, ByteAcc, NoLock, Preserve) {
	CMTI,      8,
	/* Client Management Data register */
	G64E,   1,
	G64O,      1,
	G32O,      2,
	,       2,
	GPSL,     2,
}

/* GPM Port register */
OperationRegion(GPT, SystemIO, 0x00000C52, 0x00000001)
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
OperationRegion(FRE, SystemIO, 0x00000C6F, 0x00000001)
	Field(FRE, ByteAcc, NoLock, Preserve) {
	,     0x00000006,
	FLRE, 0x00000001,
}

/* PM2 index/data registers */
OperationRegion(PM2R, SystemIO, 0x00000CD0, 0x00000002)
	Field(PM2R, ByteAcc, NoLock, Preserve) {
	PM2I, 0x00000008,
	PM2D, 0x00000008,
}

/* Power Management I/O registers, TODO:PMIO is quite different in SB800. */
OperationRegion(PIOR, SystemIO, 0x00000CD6, 0x00000002)
	Field(PIOR, ByteAcc, NoLock, Preserve) {
	PIOI, 0x00000008,
	PIOD, 0x00000008,
}
IndexField (PIOI, PIOD, ByteAcc, NoLock, Preserve) {
	    , 1,	/* MiscControl */
	T1EE, 1,
	T2EE, 1,
	Offset(0x01),	/* MiscStatus */
	, 1,
	T1E, 1,
	T2E, 1,
	Offset(0x04),	/* SmiWakeUpEventEnable3 */
	, 7,
	SSEN, 1,
	Offset(0x07),	/* SmiWakeUpEventStatus3 */
	, 7,
	CSSM, 1,
	Offset(0x10),	/* AcpiEnable */
	, 6,
	PWDE, 1,
	Offset(0x1C),	/* ProgramIoEnable */
	, 3,
	MKME, 1,
	IO3E, 1,
	IO2E, 1,
	IO1E, 1,
	IO0E, 1,
	Offset(0x1D),	/* IOMonitorStatus */
	, 3,
	MKMS, 1,
	IO3S, 1,
	IO2S, 1,
	IO1S, 1,
	IO0S,1,
	Offset(0x20),	/* AcpiPmEvtBlk. TODO: should be 0x60 */
	APEB, 16,
	Offset(0x36),	/* GEvtLevelConfig */
	, 6,
	ELC6, 1,
	ELC7, 1,
	Offset(0x37),	/* GPMLevelConfig0 */
	, 3,
	PLC0, 1,
	PLC1, 1,
	PLC2, 1,
	PLC3, 1,
	PLC8, 1,
	Offset(0x38),	/* GPMLevelConfig1 */
	, 1,
	 PLC4, 1,
	 PLC5, 1,
	, 1,
	 PLC6, 1,
	 PLC7, 1,
	Offset(0x3B),	/* PMEStatus1 */
	GP0S, 1,
	GM4S, 1,
	GM5S, 1,
	APS, 1,
	GM6S, 1,
	GM7S, 1,
	GP2S, 1,
	STSS, 1,
	Offset(0x55),	/* SoftPciRst */
	SPRE, 1,
	, 1,
	, 1,
	PNAT, 1,
	PWMK, 1,
	PWNS, 1,

	Offset(0x65),	/* UsbPMControl */
	, 4,
	URRE, 1,
	Offset(0x68),	/* MiscEnable68 */
	, 3,
	TMTE, 1,
	, 1,
	Offset(0x92),	/* GEVENTIN */
	, 7,
	E7IS, 1,
	Offset(0x96),	/* GPM98IN */
	G8IS, 1,
	G9IS, 1,
	Offset(0x9A),	/* EnhanceControl */
	,7,
	HPDE, 1,
	Offset(0xA8),	/* PIO7654Enable */
	IO4E, 1,
	IO5E, 1,
	IO6E, 1,
	IO7E, 1,
	Offset(0xA9),	/* PIO7654Status */
	IO4S, 1,
	IO5S, 1,
	IO6S, 1,
	IO7S, 1,
}

/* PM1 Event Block
* First word is PM1_Status, Second word is PM1_Enable
*/
OperationRegion(P1EB, SystemIO, APEB, 0x04)
	Field(P1EB, ByteAcc, NoLock, Preserve) {
	TMST, 1,
	,    3,
	BMST,    1,
	GBST,   1,
	Offset(0x01),
	PBST, 1,
	, 1,
	RTST, 1,
	, 3,
	PWST, 1,
	SPWS, 1,
	Offset(0x02),
	TMEN, 1,
	, 4,
	GBEN, 1,
	Offset(0x03),
	PBEN, 1,
	, 1,
	RTEN, 1,
	, 3,
	PWDA, 1,
}
