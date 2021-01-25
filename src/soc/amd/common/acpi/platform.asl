/* SPDX-License-Identifier: GPL-2.0-only */

/* Callback methods to be implemented by mainboard */
External(\_SB.MPTS, MethodObj)
External(\_SB.MWAK, MethodObj)
External(\_SB.MINI, MethodObj)

Scope (\_SB){
	/* Platform initialization methods */
	Method (_INI, 0, NotSerialized)
	{
		If (CondRefOf (\_SB.MINI)) {
			\_SB.MINI()
		}
	}
}

/* Platform-wide wake methods */
Method (\_WAK, 1, NotSerialized)
{
	PNOT ()

	If (CondRefOf (\_SB.MWAK)) {
		\_SB.MWAK()
	}
	Return (Package (){ 0, 0 })
}

/* Platform-wide Put To Sleep (suspend) methods */
Method (\_PTS, 1, NotSerialized)
{
	If (CondRefOf (\_SB.MPTS)) {
		\_SB.MPTS()
	}
}
