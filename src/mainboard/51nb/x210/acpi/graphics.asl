/* SPDX-License-Identifier: GPL-2.0-or-later */

Scope (GFX0)
{
	Method (_DOS, 1, NotSerialized)
	{
		/* We never do anything in firmware, so _DOS is a noop */
	}
	Method (_DOD, 0, NotSerialized)
	{
		return (Package (0x03)
		{
			0x80000410, /* LCD */
			0x80000120, /* VGA */
			0x80000330 /* DP */
		})
	}
	Device (LCD)
	{
		Method (_ADR, 0, Serialized)
		{
			Return (0x800000410)
		}

		Method (_BCL, 0, NotSerialized)
		{
			Return (Package (0x12)
			{
				0x0A,
				0x0F,
				0x00,
				0x01,
				0x02,
				0x03,
				0x04,
				0x05,
				0x06,
				0x07,
				0x08,
				0x09,
				0x0A,
				0x0B,
				0x0C,
				0x0D,
				0x0E,
				0x0F
			})
		}
		Method (_BCM, 1, NotSerialized)
		{
			\_SB.PCI0.LPCB.EC.BKLG = Arg0
		}
		Method (_BQC, 0, NotSerialized)
		{
			Return (\_SB.PCI0.LPCB.EC.BKLG)
		}
	}
}
