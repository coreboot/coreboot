/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

/* WARNING
 * Switchable graphics not yet tested!
 */

/* Hybrid graphics enable/disable GPIO bitfields */
Name (HYG1, 0x004A0000)
Name (HYG2, 0x00020000)

/* GPIO control port */
Name (GPCP, DEFAULT_GPIOBASE)

/* GPIO control map */
OperationRegion (GPCM, SystemIO, GPCP, 0x3F)
Field (GPCM, ByteAcc, NoLock, Preserve) {
	GPUS, 32,
	GPIS, 32,
	Offset (0x0C),
	GPLV, 32,
	Offset (0x30),
	GQUS, 32,
	GQIS, 32,
	GQLV, 32
}

Method(SHYB, 1) {
	/* Switch hybrid graphics */
	if (LEqual(Arg0, One))
	{
		/* Discrete graphics requested */
		Or(GPLV, HYG1, GPLV)
		Or(GQLV, HYG2, GQLV)
	}
	else
	{
		/* Integrated graphics requested */
		Xor(HYG1, 0xFFFFFFFF, Local0)
		And(GPLV, Local0, GPLV)
		Xor(HYG2, 0xFFFFFFFF, Local0)
		And(GQLV, Local0, GQLV)
	}
}

Method (ATPX, 2, NotSerialized) {
	/* Create local variables */
	Name (ATPR, Buffer (0x08) {
		0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0
	})
	CreateWordField (ATPR, 0x00, SIZE)
	CreateWordField (ATPR, 0x02, VERS)
	CreateDWordField (ATPR, 0x02, MASK)
	CreateDWordField (ATPR, 0x04, FUNC)
	CreateDWordField (ATPR, 0x06, FLAG)

	/* Version request */
	if (LEqual(Arg0, 0x0))
	{
		/* Assemble and return version information */
		Store (0x08, SIZE)	/* Response length */
		Store (0x01, VERS)	/* Version number */
		Store (0x0F, FUNC)	/* Supported functions? */
		Return (ATPR)
	}

	/* Mux select */
	if (LEqual(Arg0, 0x2))
	{
		CreateByteField (Arg1, 0x02, PWST)
		Store (PWST, Local0)
		And (Local0, 0x01, Local0)
		If (Local0)
		{
			/* Enable discrete graphics */
			SHYB(0x01)
		}
		else
		{
			/* Enable integrated graphics */
			SHYB(0x00)
		}
	}
}
