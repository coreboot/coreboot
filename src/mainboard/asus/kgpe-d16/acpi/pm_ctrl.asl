/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Raptor Engineering
 * Copyright (C) 2009 Advanced Micro Devices, Inc.
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

/* Port 80 POST card debug */
OperationRegion (DBG0, SystemIO, 0x80, One)
	Field (DBG0, ByteAcc, NoLock, Preserve) {
	DBG8, 8
}

/* SuperIO control port */
Name (SPIO, 0x2E)

/* SuperIO control map */
OperationRegion (SPIM, SystemIO, SPIO, 0x02)
	Field (SPIM, ByteAcc, NoLock, Preserve) {
	INDX, 8,
	DATA, 8
}

/* SuperIO control registers */
IndexField (INDX, DATA, ByteAcc, NoLock, Preserve) {
	Offset (0x07),
	CR07, 8,		/* Logical device number */
	Offset (0x2C),
	CR2C, 8,		/* GPIO3 multiplexed pin selection */
	Offset (0x30),
	CR30, 8,		/* Logical device activation control register */
	Offset (0xE0),
	CRE0, 8,		/* Wake control register */
	Offset (0xE4),
	CRE4, 8,		/* Standby power control register */
	Offset (0xE6),
	CRE6, 8,		/* Mouse wake event configuration register */
	Offset (0xF1),
	CRF1, 8,		/* GPIO3 data register */
	Offset (0xF3),
	CRF3, 8,		/* SUSLED mode register */
	Offset (0xF6),
	CRF6, 8,		/* SMI/PME event generation control register */
	Offset (0xF9),
	CRF9, 8,		/* ACPI PME configuration register */
}

/* Power Management I/O registers */
OperationRegion(PIOR, SystemIO, 0x00000CD6, 0x00000002)
	Field(PIOR, ByteAcc, NoLock, Preserve) {
	PIOI, 0x00000008,
	PIOD, 0x00000008,
}
IndexField (PIOI, PIOD, ByteAcc, NoLock, Preserve) {
	Offset(0x00),	/* MiscControl */
	, 1,
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
	Offset(0x20),	/* AcpiPmEvtBlk */
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

	/*	Offset(0x61), */	/*  Options_1 */
	/*		,7,  */
	/*		R617,1, */

	Offset(0x65),	/* UsbPMControl */
	, 4,
	URRE, 1,
	, 2,
	BCDL, 1,
	Offset(0x68),	/* MiscEnable68 */
	, 2,
	MAPC, 1,
	TMTE, 1,
	, 1,
	Offset(0x7C),	/* MiscEnable7C */
	, 2,
	BLNK, 2,
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

/* Wake status package */
Name(WKST,Package() {Zero, Zero})

/*
 *  \_WAK System Wake method
 *
 *	Entry:
 *		Arg0=The value of the sleeping state S1=1, S2=2
 *
 *	Exit:
 *		Return package of 2 DWords
 *		Dword 1 - Status
 *			0x00000000	wake succeeded
 *			0x00000001	Wake was signaled but failed due to lack of power
 *			0x00000002	Wake was signaled but failed due to thermal condition
 *		Dword 2 - Power Supply state
 *			if non-zero the effective S-state the power supply entered
 */
Method(\_WAK, 1) {
	Store (0x20, DBG8)

	/* Set up LEDs */
	/* Set power LED to steady on */
	Store(0x0, BLNK)

	/* Configure SuperIO for wake */
	/* Access SuperIO ACPI device */
	Store(0x87, INDX)
	Store(0x87, INDX)
	Store(0x0A, CR07)

	if (LEqual(Arg0, One))	/* Resuming from power state S1 */
	{
		/* Deactivate the ACPI device */
		Store(Zero, CR30)

		/* Disable PS/2 SMI/PME events */
		And(CRF6, 0xCF, CRF6)
	}
	if (Lor(LEqual(Arg0, 0x03), LEqual(Arg0, 0x04)))	/* Resuming from power state S3 or S4 */
	{
		/* Disable PS/2 wake */
		And(CRE0, 0x1D, CRE0)
		And(CRE6, 0x7F, CRE6)
	}

	/* Restore default SuperIO access */
	Store(0xAA, INDX)

	Store (0x21, DBG8)

	/* Re-enable HPET */
	Store(1, HPDE)

	/* Restore PCIRST# so it resets USB */
	if (LEqual(Arg0, 3)){
		Store(1, URRE)
	}

	/* Configure southbridge for wake */
	/* Arbitrarily clear PciExpWakeStatus */
	Store(PWST, Local1)
	Store(Local1, PWST)

	Store (0x22, DBG8)

	Notify(\_SB.PWRB, 0x02)			/* NOTIFY_DEVICE_WAKE */

	Return(WKST)
}

/*
 * \_PTS - Prepare to Sleep method
 *
 *	Entry:
 *		Arg0=The value of the sleeping state S1=1, S2=2, etc
 *
 * Exit:
 *		-none-
 *
 * The _PTS control method is executed at the beginning of the sleep process
 * for S1-S5. The sleeping value is passed to the _PTS control method.  This
 * control method may be executed a relatively long time before entering the
 * sleep state and the OS may abort the operation without notification to
 * the ACPI driver.  This method cannot modify the configuration or power
 * state of any device in the system.
 */
Method(\_PTS, 1) {
	Store (Arg0, DBG8)

	/* Set up LEDs */
	if (LEqual(Arg0, One))	/* Power state S1 requested */
	{
		/* Set suspend LED to 0.25Hz toggle pulse with 50% duty cycle */
		Store(0x2, BLNK)
	}

	/* Configure SuperIO for sleep */
	/* Access SuperIO ACPI device */
	Store(0x87, INDX)
	Store(0x87, INDX)
	Store(0x0A, CR07)

	/* Disable PS/2 wakeup and connect PANSW_IN to PANSW_OUT */
	And(CRE0, 0x1F, CRE0)

	if (LEqual(Arg0, One))	/* Power state S1 requested */
	{
		/* Activate the ACPI device */
		Store(One, CR30)

		/* Disable SMI/PME events for:
			* LPT
			* FDC
			* UART
			*/
		Store(0x00, CRF6)

		/* Enable PS/2 keyboard SMI/PME events */
		Or(CRF6, 0x10, CRF6)

		/* Enable PS/2 keyboard wake */
		Or(CRE0, 0x40, CRE0)

		/* Enable PS/2 mouse SMI/PME events */
		Or(CRF6, 0x20, CRF6)

		/* Enable PS/2 mouse wake  */
		Or(CRE0, 0x20, CRE0)
	} else {
		/* Enable PS/2 keyboard wake on any keypress */
		Or(CRE0, 0x41, CRE0)

		/* Enable PS/2 mouse wake on any click  */
		Or(CRE0, 0x22, CRE0)
		Or(CRE6, 0x80, CRE6)

		if (LEqual(Arg0, 0x03))	/* Power state S3 requested */
		{
			/* Set VSBGATE# to provide standby power during S3 */
			Or(CRE4, 0x10, CRE4)
		}
	}

	/* Restore default SuperIO access */
	Store(0xAA, INDX)

	Store (0x10, DBG8)

	/* Don't allow PCIRST# to reset USB */
	if (LEqual(Arg0, 3)){
		Store(0, URRE)
	}

	/* Configure southbridge for sleep */
	/* Use bus clock for delay timebase */
	Store(0, BCDL)
	/* Defer APIC interrupts until first ACPI access */
	Store(One, MAPC)

	/* On older chips, clear PciExpWakeDisEn */
	// if (LLessEqual(SBRI, 0x13)) {
	//	Store(0, PWDE)
	// }

	Store (0x11, DBG8)

	/* Clear wake status structure. */
	Store(0, Index(WKST,0))
	Store(0, Index(WKST,1))
}
