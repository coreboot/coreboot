/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel PCH HDA */

// Intel High Definition Audio (Azalia) 0:1b.0

Device (HDEF)
{
	Name (_ADR, 0x001b0000)
	Name (_PRW, Package () { DEFAULT_PRW_VALUE, 3 })
}
