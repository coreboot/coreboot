/* SPDX-License-Identifier: GPL-2.0-only */

/*
Scope(\_SB) {
	#include "globutil.asl"
}
*/

/* string compare functions */
Method(MIN, 2)
{
	if (Arg0 < Arg1) {
		Return(Arg0)
	} else {
		Return(Arg1)
	}
}

Method(SLEN, 1)
{
	Local0 = Arg0
	Return(Sizeof(Local0))
}

Method(S2BF, 1, Serialized)
{
	Local0 = SLEN(Arg0) + 1
	Name(BUFF, Buffer(Local0) {})
	BUFF = Arg0
	Return(BUFF)
}

/* Strong string compare.  Checks both length and content */
Method(SCMP, 2)
{
	Local0 = S2BF(Arg0)
	Local1 = S2BF(Arg1)
	Local4 = 0
	Local5 = SLEN(Arg0)
	Local6 = SLEN(Arg1)
	Local7 = MIN(Local5, Local6)

	While(Local4 < Local7) {
		Local2 = Derefof(Local0[Local4])
		Local3 = Derefof(Local1[Local4])
		if (Local2 > Local3) {
			Return(One)
		} else {
			if (Local2 < Local3) {
				Return(Ones)
			}
		}
		Local4++
	}
	if (Local4 < Local5) {
		Return(One)
	} else {
		if (Local4 < Local6) {
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
	Local0 = S2BF(Arg0)
	Local1 = S2BF(Arg1)
	if (SLEN(Arg0) < SLEN(Arg1)) {
		Return(0)
	}
	Local2 = 0
	Local3 = SLEN(Arg1)

	While(Local2 < Local3) {
		if (Derefof(Local0[Local2]) != Derefof(Local1[Local2])) {
			Return(0)
		}
		Local2++
	}
	Return(One)
}

/* ARG0 = IRQ Number(0-15)
* Returns Bit Map
*/
Method(I2BM, 1)
{
	Local0 = 0
	if (ARG0 != 0) {
		Local1 = 1
		Local0 = Local1 << ARG0
	}
	Return(Local0)
}
