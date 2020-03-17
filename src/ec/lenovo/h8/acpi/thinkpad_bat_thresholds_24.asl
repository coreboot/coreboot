/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


Scope(\_SB.PCI0.LPCB.EC)
{
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0x03),
				, 2,
				BSTP, 1, /* Battery start/stop threshold */
		Offset (0x24),
				TSH0, 8, /* Battery0 threshold */
		Offset (0x25),
				TSH1, 8, /* Battery1 threshold */
	}
}

Scope(\_SB.PCI0.LPCB.EC.BAT0)
{
	/*
	 * Set threshold on battery0,
	 *
	 * Arg0: 0: Start threshold
	 *       1: Stop threshold
	 * Arg1: Percentage
	 */
	Method (SETT, 2, NotSerialized)
	{
		if (Arg0 <= 1 && Arg1 <= 100)
		{
			BSTP = Arg0
#if defined(H8_BAT_THRESHOLDS_BIT7)
			TSH0 = Arg1
#else
			TSH0 = Arg1 | 0x80
#endif
		}
	}

	/**
	 * Get threshold on battery0
	 *
	 * Arg0: 0: Start threshold
	 *       1: Stop threshold
	 */
	Method (GETT, 1, NotSerialized)
	{
		if (Arg0 <= 1)
		{
			BSTP = Arg0
#if defined(H8_BAT_THRESHOLDS_BIT7)
			Return (TSH0)
#else
			Return (TSH0 & ~0x80)
#endif
		}
		Return (0)
	}
}

Scope(\_SB.PCI0.LPCB.EC.BAT1)
{
	/*
	 * Set threshold on battery1
	 *
	 * Arg0: 0: Start threshold
	 *       1: Stop threshold
	 * Arg1: Percentage
	 */
	Method (SETT, 2, NotSerialized)
	{
		if (Arg0 <= 1 && Arg1 <= 100)
		{
			BSTP = Arg0
#if defined(H8_BAT_THRESHOLDS_BIT7)
			TSH1 = Arg1
#else
			TSH1 = Arg1 | 0x80
#endif
		}
	}

	/**
	 * Get threshold on battery1
	 *
	 * Arg0: 0: Start threshold
	 *       1: Stop threshold
	 */
	Method (GETT, 1, NotSerialized)
	{
		if (Arg0 <= 1)
		{
			BSTP = Arg0
#if defined(H8_BAT_THRESHOLDS_BIT7)
			Return (TSH1)
#else
			Return (TSH1 & ~0x80)
#endif
		}
		Return (0)
	}
}
