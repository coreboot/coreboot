/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Kyösti Mälkki <kyosti.malkki@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


/* Board powers on with button or PME# from on-board GbE wake-on-lan.
 * Board shuts down to S5/G2. Any other power management is untested.
 */

Name (\_S0, Package () { 0x00, 0x00, 0x00, 0x00 })
Name (\_S1, Package () { 0x01, 0x01, 0x00, 0x00 })
Name (\_S3, Package () { 0x05, 0x05, 0x00, 0x00 })
Name (\_S4, Package () { 0x06, 0x06, 0x00, 0x00 })
Name (\_S5, Package () { 0x07, 0x07, 0x00, 0x00 })

Scope (\_GPE)
{
	Method (_L03, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.USB0, 0x02)
	}
	Method (_L04, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.USB1, 0x02)
	}

	/* WOL header */
	Method (_L08, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.PCI5, 0x02)
		Notify (\_SB.SLBT, 0x02)
	}

	/* PME# */
	Method (_L0B, 0, NotSerialized)
	{
#if 1
		Notify (\_SB.LID0, 0x02)
#else
		Notify (\_SB.PCI0.HLIB.P64B.ETH0, 0x02)
		Notify (\_SB.PCI0.HLIB.P64B, 0x02)
		Notify (\_SB.PCI0.HLIB.P64A, 0x02)
#endif
	}

	Method (_L0C, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.USB2, 0x02)
	}

	/* PME_B0_STS# */
	Method (_L0D, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.USB3, 0x02)
	}
}

/* Clear power buttons */
Method (\_INI, 0, NotSerialized)
{
	Or (\_SB.PCI0.ICH0.PS1H, 0x09, \_SB.PCI0.ICH0.PS1H)
	Or (\_SB.PCI0.ICH0.PE1H, 0x01, \_SB.PCI0.ICH0.PE1H)
}

/* Prepare To Sleep */
Method (\_PTS, 1, NotSerialized)
{
	Or (\_SB.PCI0.ICH0.GS0H, 0x19, \_SB.PCI0.ICH0.GS0H)
	Or (\_SB.PCI0.ICH0.GS0L, 0x11, \_SB.PCI0.ICH0.GS0L)
}

/* System Wake */
Method (\_WAK, 1, NotSerialized)
{
	Or (\_SB.PCI0.ICH0.GS0H, 0x19, \_SB.PCI0.ICH0.GS0H)
	Or (\_SB.PCI0.ICH0.GS0L, 0x11, \_SB.PCI0.ICH0.GS0L)

	Return ( Package() { 0x0, 0x0 } )
}

