/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
