/* SPDX-License-Identifier: GPL-2.0-or-later */

#define R_ICLK_PCR_CAMERA1	0x8000
#define B_ICLK_PCR_FREQUENCY	0x1
#define B_ICLK_PCR_REQUEST	0x2

/* The clock control registers for each IMGCLK are offset by 0xC */
#define B_ICLK_PCR_OFFSET	0xC

Scope (\_SB.PCI0) {

	/* IsCLK PCH base register for clock settings */
	Name (ICKB, 0)
	Store (PCRB (PID_ISCLK) + R_ICLK_PCR_CAMERA1, ICKB)

	/*
	 * Arg0 : Clock Number
	 * Return : Offset of register to control the clock in Arg0
	 *
	 */
	Method (OFST, 0x1, NotSerialized)
	{
		Return (ICKB + (Arg0 * B_ICLK_PCR_OFFSET))
	}

	/*
	 * Helper function for Read And OR Write
	 * Arg0 : source and destination
	 * Arg1 : And data
	 * Arg2 : Or data
	 */
	Method (RAOW, 0x3, Serialized)
	{
		OperationRegion (ICLK, SystemMemory, OFST(Arg0), 4)
		Field (ICLK, AnyAcc, NoLock, Preserve)
		{
			VAL0, 32
		}
		Local0 = VAL0
		VAL0 = Local0 & Arg1 | Arg2
	}

	/*
	 * Clock control Method
	 * Arg0: Clock source select(0: IMGCLKOUT_0, 1: IMGCLKOUT_1, 2: IMGCLKOUT_2, 3: IMGCLKOUT_3,
	 *                           4: IMGCLKOUT_4, 5: IMGCLKOUT_5)
	 * Arg1: Select 24MHz / 19.2 MHz (0: 24MHz, 1: 19.2MHz)
	 */
	Method (MCON, 0x2, NotSerialized)
	{
		/* Set Clock Frequency */
		RAOW (Arg0, ~B_ICLK_PCR_FREQUENCY, Arg1)

		/* Enable Clock */
		RAOW (Arg0, ~B_ICLK_PCR_REQUEST, B_ICLK_PCR_REQUEST)
	}

	Method (MCOF, 0x1, NotSerialized)
	{
		/* Disable Clock */
		RAOW (Arg0, ~B_ICLK_PCR_REQUEST, 0)
	}
}
