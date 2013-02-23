/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
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

/*
Scope(\_SB) {
	#include "globutil.asl"
}
*/

/* string compare functions */
Method(MIN, 2)
{
	if (LLess(Arg0, Arg1)) {
		Return(Arg0)
	} else {
		Return(Arg1)
	}
}

Method(SLEN, 1)
{
	Store(Arg0, Local0)
	Return(Sizeof(Local0))
}

Method(S2BF, 1)
{
	Add(SLEN(Arg0), One, Local0)
	Name(BUFF, Buffer(Local0) {})
	Store(Arg0, BUFF)
	Return(BUFF)
}

/* Strong string compare.  Checks both length and content */
Method(SCMP, 2)
{
	Store(S2BF(Arg0), Local0)
	Store(S2BF(Arg1), Local1)
	Store(Zero, Local4)
	Store(SLEN(Arg0), Local5)
	Store(SLEN(Arg1), Local6)
	Store(MIN(Local5, Local6), Local7)

	While(LLess(Local4, Local7)) {
		Store(Derefof(Index(Local0, Local4)), Local2)
		Store(Derefof(Index(Local1, Local4)), Local3)
		if (LGreater(Local2, Local3)) {
			Return(One)
		} else {
			if (LLess(Local2, Local3)) {
				Return(Ones)
			}
		}
		Increment(Local4)
	}
	if (LLess(Local4, Local5)) {
		Return(One)
	} else {
		if (LLess(Local4, Local6)) {
			Return(Ones)
		} else {
			Return(Zero)
		}
	}
}

/* Weak string compare.  Checks to find Arg1 at beginning of Arg0.
* Fails if length(Arg0) < length(Arg1).  Returns 0 on Fail, 1 on
* Pass.
*/
Method(WCMP, 2)
{
	Store(S2BF(Arg0), Local0)
	Store(S2BF(Arg1), Local1)
	if (LLess(SLEN(Arg0), SLEN(Arg1))) {
		Return(0)
	}
	Store(Zero, Local2)
	Store(SLEN(Arg1), Local3)

	While(LLess(Local2, Local3)) {
		if (LNotEqual(Derefof(Index(Local0, Local2)),
			Derefof(Index(Local1, Local2)))) {
			Return(0)
		}
		Increment(Local2)
	}
	Return(One)
}

/* ARG0 = IRQ Number(0-15)
* Returns Bit Map
*/
Method(I2BM, 1)
{
	Store(0, Local0)
	if (LNotEqual(ARG0, 0)) {
		Store(1, Local1)
		ShiftLeft(Local1, ARG0, Local0)
	}
	Return(Local0)
}
Method (SEQL, 2, Serialized)
{
	Store (SizeOf (Arg0), Local0)
	Store (SizeOf (Arg1), Local1)
	If (LNot (LEqual (Local0, Local1))) { Return (Zero) }

	Name (BUF0, Buffer (Local0) {})
	Store (Arg0, BUF0)
	Name (BUF1, Buffer (Local0) {})
	Store (Arg1, BUF1)
	Store (Zero, Local2)
	While (LLess (Local2, Local0))
	{
		Store (DerefOf (Index (BUF0, Local2)), Local3)
		Store (DerefOf (Index (BUF1, Local2)), Local4)
		If (LNot (LEqual (Local3, Local4))) { Return (Zero) }

		Increment (Local2)
	}

	Return (One)
}

/* GetMemoryResources(Node, Link) */
Method (GMEM, 2, NotSerialized)
{
	Name (BUF0, ResourceTemplate ()
	{
		DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
			0x00000000, // Address Space Granularity
			0x00000000, // Address Range Minimum
			0x00000000, // Address Range Maximum
			0x00000000, // Address Translation Offset
			0x00000001,,,
			, AddressRangeMemory, TypeStatic)
	})
	CreateDWordField (BUF0, 0x0A, MMIN)
	CreateDWordField (BUF0, 0x0E, MMAX)
	CreateDWordField (BUF0, 0x16, MLEN)
	Store (0x00, Local0)
	Store (0x00, Local4)
	Store (0x00, Local3)
	While (LLess (Local0, 0x10))
	{
		/* Get value of the first register */
		Store (DerefOf (Index (\_SB.PCI0.MMIO, Local0)), Local1)
		Increment (Local0)
		Store (DerefOf (Index (\_SB.PCI0.MMIO, Local0)), Local2)
		If (LEqual (And (Local1, 0x03), 0x03)) /* Pair enabled? */
		{
			If (LEqual (Arg0, And (Local2, 0x07))) /* Node matches? */
			{
				/* If Link Matches (or we got passed 0xFF) */
				If (LOr (LEqual (Arg1, 0xFF), LEqual (Arg1, ShiftRight (And (Local2, 0x30), 0x04))))
				{
					/* Extract the Base and Limit values */
					Store (ShiftLeft (And (Local1, 0xFFFFFF00), 0x08), MMIN)
					Store (ShiftLeft (And (Local2, 0xFFFFFF00), 0x08), MMAX)
					Or (MMAX, 0xFFFF, MMAX)
					Subtract (MMAX, MMIN, MLEN)
					Increment (MLEN)

					If (Local4) /* I've already done this once */
					{
						Concatenate (RTAG (BUF0), Local3, Local5)
						Store (Local5, Local3)
					}
					Else
					{
						Store (RTAG (BUF0), Local3)
					}

					Increment (Local4)
				}
			}
		}

		Increment (Local0)
	}

	If (LNot (Local4)) /* No resources for this node and link. */
	{
		Store (RTAG (BUF0), Local3)
	}

	Return (Local3)
}

Method (RTAG, 1, NotSerialized)
{
	Store (Arg0, Local0)
	Store (SizeOf (Local0), Local1)
	Subtract (Local1, 0x02, Local1)
	Multiply (Local1, 0x08, Local1)
	CreateField (Local0, 0x00, Local1, RETB)
	Store (RETB, Local2)
	Return (Local2)
}
