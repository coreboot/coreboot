/* SPDX-License-Identifier: GPL-2.0-or-later */

#define R_ICLK_PCR_CAMERA1	0x8000
#define B_ICLK_PCR_FREQUENCY	0x3
#define B_ICLK_PCR_REQUEST	0x4

/* The clock control registers for each IMGCLK are offset by 0xC */
#define B_ICLK_PCR_OFFSET	0xC

Scope (\_SB.PCI0) {

	/* IsCLK PCH base register for clock settings */
	Name (ICKB, 0)
	ICKB = PCRB (PID_ISCLK) + R_ICLK_PCR_CAMERA1
	/*
	 * Helper function for Read And Or Write
	 * Arg0 : Clock source select
	 * Arg1 : And data
	 * Arg2 : Or data
	 */
	Method (RAOW, 3, Serialized)
	{
		OperationRegion (ICLK, SystemMemory, (ICKB + (Arg0 * B_ICLK_PCR_OFFSET)), 4)
		Field (ICLK, AnyAcc, NoLock, Preserve)
			{
			VAL0, 32
		}
		Local0 = VAL0
		VAL0 = Local0 & Arg1 | Arg2
	}

	/*
	 * Clock control Method
	 * Arg0: Clock source select (0 .. 5 => IMGCLKOUT_0 .. IMGCLKOUT_5)
	 * Arg1: Frequency select
	 *  2'b00 - 19p2 XTAL
	 *  2'b01 - 19p2 IMG
	 *  2'b10 - 19p2 RTC
	 *  2'b11 - 24 IMG
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
