/* SPDX-License-Identifier: GPL-2.0-only */

External (\_SB.RPTS, MethodObj)
External (\_SB.RWAK, MethodObj)

Method (MPTS, 1, NotSerialized)
{
	If (Arg0)
	{
		\_SB.RPTS (Arg0)
	}
}

Method (MWAK, 1, NotSerialized)
{
	\_SB.RWAK (Arg0)
	Return (0x00)
}
