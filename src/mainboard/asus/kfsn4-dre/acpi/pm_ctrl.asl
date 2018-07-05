/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2010 - 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2004 Nick Barker <Nick.Barker9@btinternet.com>
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
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
 * WARNING: Sleep/Wake is a work in progress and is still somewhat flaky!
 */

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

	/* Southbridge control ports */
	/* Both are offsets from PM base address (0x2000) */
	Name (SBC1, 0x2090)	/* Offset 0x90 */
	Name (SBC2, 0x2400)	/* Offset 0x400 */

	/* Southbridge control maps */
	OperationRegion (SBM1, SystemIO, SBC1, 0x10)
		Field (SBM1, ByteAcc, NoLock, Preserve) {
		S1CT, 2,
		Offset (0x04),
		S3CT, 2,
		Offset (0x08),
		S4CT, 2,
		Offset (0x0C),
		S5CT, 2,
	}
	OperationRegion (SBM2, SystemIO, SBC2, 0x08)
		Field (SBM2, ByteAcc, NoLock, Preserve) {
		, 15,
		PS1S, 1,
		, 31,
		PS1E, 1,
	}

	/* Wake status package */
	Name(WKST,Package(){Zero, Zero})

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
		/* Set up LEDs */
		/* Access SuperIO GPIO3/GPIO4 device */
		Store(0x87, INDX)
		Store(0x87, INDX)
		Store(0x09, CR07)

		/* Set GPIO3 pin 64 (power LED) to GP37 mode */
		And(CR2C, 0xF3, Local0)
		Or(Local0, 0x04, CR2C)

		/* Set power LED to steady on */
		Or(CRF1, 0x80, CRF1)

		/* Restore default SuperIO access */
		Store(0xAA, INDX)

		/* Configure SuperIO for wake */
		/* Access SuperIO ACPI device */
		Store(0x87, INDX)
		Store(0x87, INDX)
		Store(0x0A, CR07)

		if (LEqual(Arg0, One))	/* Resuming from power state S1 */
		{
			/* Set power management to SMI mode and disable SMI events */
			And(CRF9, 0xFA, CRF9)

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

		/* Configure southbridge for wake */
		Store(Zero, PS1E)
		Store(0x02, S1CT)
		Store(0x02, S3CT)
		Store(0x02, S4CT)
		Store(0x02, S5CT)
		Notify(\_SB.PWRB, 0x02)			/* NOTIFY_DEVICE_WAKE */

		Return(WKST)
	} /* End Method(\_WAK) */

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
		/* Set up LEDs */
		if (LEqual(Arg0, One))	/* Power state S1 requested */
		{
			/* Access SuperIO GPIO3/GPIO4 device */
			Store(0x87, INDX)
			Store(0x87, INDX)
			Store(0x09, CR07)

			/* Set GPIO3 pin 64 (power LED) to SUSLED mode */
			And(CR2C, 0xF3, CR2C)

			/* Set suspend LED to 1Hz toggle pulse with 50% duty cycle */
			Or(CRF3, 0x80, CRF3)

			/* Restore default SuperIO access */
			Store(0xAA, INDX)
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
			/* Set power management to PME mode and enable PME events */
			Or(CRF9, 0x05, CRF9)

			/* Activate the ACPI device */
			Store(One, CR30)

			/* Enable PS/2 keyboard SMI/PME events */
			And(CRF6, 0xEF, CRF6)

			/* Enable PS/2 keyboard wake */
			Or(CRE0, 0x40, CRE0)

			/* Enable PS/2 mouse SMI/PME events */
			And(CRF6, 0xDF, CRF6)

			/* Enable PS/2 mouse wake  */
			Or(CRE0, 0x20, CRE0)
		}
		else {
			/* Enable PS/2 keyboard wake on any keypress */
			Or(CRE0, 0x41, CRE0)

			/* Enable PS/2 mouse wake on any click  */
			Or(CRE0, 0x22, CRE0)
			Or(CRE6, 0x80, CRE6)
		}

		/* Restore default SuperIO access */
		Store(0xAA, INDX)

		/* Configure southbridge for sleep */
		Store(One, PS1S)
		Store(One, PS1E)

		/* On older chips, clear PciExpWakeDisEn */
		/*if (LLessEqual(\_SB.SBRI, 0x13)) {
		*	Store(0,\_SB.PWDE)
		*}
		*/

		/* Clear wake status structure. */
		Store(0, Index(WKST,0))
		Store(0, Index(WKST,1))
	} /* End Method(\_PTS) */
