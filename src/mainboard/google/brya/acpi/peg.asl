/* SPDX-License-Identifier: GPL-2.0-or-later */

External (\_SB.PCI0.DGPU, DeviceObj)
External (\_SB.PCI0.PEG0.PEGP._OFF, MethodObj)
External (\_SB.PCI0.PEG0.PEGP._ON, MethodObj)

OperationRegion (PCIC, PCI_Config, 0x00, 0x100)
Field (PCIC, AnyAcc, NoLock, Preserve)
{
	Offset (0x52),
	,	13,
	LASX,	1,		/* Link Active Status */
	Offset (0xe0),
	,	7,
	NCB7,	1,		/* Scratch bit to save L2/3 state */
	Offset (0xe2),
	,	2,
	L23E,	1,		/* L23_Rdy Entry request */
	L23R,	1		/* L23_Rdy Detect Transition */
}

/* L2/3 Entry sequence */
Method (DL23, 0, Serialized)
{
	L23E = 1
	Local0 = 8
	While (Local0 > 0)
	{
		If (!L23E)
		{
			Break
		}

		Sleep (16)
		Local0--
	}
	NCB7 = 1
}

/* L2/3 exit seqeuence */
Method (LD23, 0, Serialized)
{
	If (!NCB7)
	{
		Return
	}

	L23R = 1
	Local0 = 20
	While (Local0 > 0)
	{
		If (!L23R)
		{
			Break
		}

		Sleep (16)
		Local0--
	}

	NCB7 = 0
	Local0 = 8
	While (Local0 > 0)
	{
		If (LASX == 1)
		{
			Break
		}

		Sleep (16)
		Local0--
	}
}

/* PEG Power Resource */
PowerResource (PGPR, 0, 0)
{
	Method (_ON, 0, Serialized)
	{
		/* Power up GPU from GCOFF (or GC6 exit if deferred) */
		\_SB.PCI0.PEG0.PEGP._ON ()
		_STA = 1
	}
	Method (_OFF, 0, Serialized)
	{
		/* Power down GPU to GCOFF (or GC6 entry if deferred) */
		_STA = 0
		\_SB.PCI0.PEG0.PEGP._OFF ()
	}
	Name (_STA, 0)
}

Name (_PR0, Package() { PGPR })
Name (_PR2, Package() { PGPR })
Name (_PR3, Package() { PGPR })
