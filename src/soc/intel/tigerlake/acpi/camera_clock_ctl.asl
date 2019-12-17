/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2020 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define R_ICLK_PCR_CAMERA1	0x8000
#define B_ICLK_PCR_FREQUENCY	0x1
#define B_ICLK_PCR_REQUEST	0x2

Scope (\_SB.PCI0) {
	/* IsCLK PCH register for clock settings */
	OperationRegion (ICLK, SystemMemory, PCRB (PID_ISCLK) + R_ICLK_PCR_CAMERA1, 0x40)
	Field (ICLK, AnyAcc, Lock, Preserve)
	{
		CLK1, 8,
		Offset(0x0C),
		CLK2, 8,
		Offset(0x18),
		CLK3, 8,
		Offset(0x24),
		CLK4, 8,
		Offset(0x30),
		CLK5, 8,
		Offset(0x3C),
		CLK6, 8,
	}

	/*
	 * Helper function for Read And OR Write
	 * Arg0 : source and destination
	 * Arg1 : And data
	 * Arg2 : Or data
	 */
	Method (RAOW, 0x3, NotSerialized)
	{
		Local0 = Arg0
		Arg0 = Local0 & Arg1 | Arg2
	}

	/*
	 * Clock Control
	 * Arg0 - Clock number (0:IMGCLKOUT_0, etc)
	 * Arg1 - Desired state (0:Disable, 1:Enable)
	 */
	Method(CLKC, 0x2, NotSerialized)
	{

		Switch (ToInteger (Arg0))
		{
			Case (0)
			{
				RAOW (CLK1, ~B_ICLK_PCR_REQUEST, Arg1 << 1)
			}
			Case (1)
			{
				RAOW (CLK2, ~B_ICLK_PCR_REQUEST, Arg1 << 1)
			}
			Case (2)
			{
				RAOW (CLK3, ~B_ICLK_PCR_REQUEST, Arg1 << 1)
			}
			Case (3)
			{
				RAOW (CLK4, ~B_ICLK_PCR_REQUEST, Arg1 << 1)
			}
			Case (4)
			{
				RAOW (CLK5, ~B_ICLK_PCR_REQUEST, Arg1 << 1)
			}
			Case (5)
			{
				RAOW (CLK6, ~B_ICLK_PCR_REQUEST, Arg1 << 1)
			}
		}
	}

	/*
	 * Clock Frequency
	 * Arg0 - Clock number (0:IMGCLKOUT_0, etc)
	 * Arg1 - Clock frequency (0:24MHz, 1:19.2MHz)
	 */
	Method (CLKF, 0x2, NotSerialized)
	{
		Switch (ToInteger (Arg0))
		{
			Case (0)
			{
				RAOW (CLK1, ~B_ICLK_PCR_FREQUENCY, Arg1)
			}
			Case (1)
			{
				RAOW (CLK2, ~B_ICLK_PCR_FREQUENCY, Arg1)
			}
			Case (2)
			{
				RAOW (CLK3, ~B_ICLK_PCR_FREQUENCY, Arg1)
			}
			Case (3)
			{
				RAOW (CLK4, ~B_ICLK_PCR_FREQUENCY, Arg1)
			}
			Case (4)
			{
				RAOW (CLK5, ~B_ICLK_PCR_FREQUENCY, Arg1)
			}
			Case (5)
			{
				RAOW (CLK6, ~B_ICLK_PCR_FREQUENCY, Arg1)
			}
		}
	}

	/*
	 * Clock control Method
	 * Arg0: Clock source select(0: IMGCLKOUT_0, 1: IMGCLKOUT_1, 2: IMGCLKOUT_2, 3: IMGCLKOUT_3,
	 *                           4: IMGCLKOUT_4, 5: IMGCLKOUT_5)
	 * Arg1: Clock Enable / Disable (0: Disable, 1: Enable)
	 * Arg2: Select 24MHz / 19.2 MHz (0: 24MHz, 1: 19.2MHz)
	 */
	Method (MCCT, 0x3, NotSerialized)
	{
		CLKF (Arg0, Arg2)
		CLKC (Arg0, Arg1)
	}
}
