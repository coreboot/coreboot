/* SPDX-License-Identifier: GPL-2.0-only */

External (\_SB.CP00._TSS, MethodObj)
External (\_SB.CP00._TPC, MethodObj)
External (\_SB.CP00._PTC, PkgObj)
External (\_SB.CP00._TSD, PkgObj)
External (\_SB.CP00._PSS, MethodObj)

Device (TCPU)
{
	Name (_HID, EISAID ("INT3401"))
	Name (_UID, 0)

	Method (_STA)
	{
		If (\DPTE == One) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	/*
	 * Processor Throttling Controls
	 */

	Method (_TSS)
	{
		If (CondRefOf (\_SB.CP00._TSS)) {
			Return (\_SB.CP00._TSS)
		} Else {
			Return (Package ()
			{
				Package () { 0, 0, 0, 0, 0 }
			})
		}
	}

	Method (_TPC)
	{
		If (CondRefOf (\_SB.CP00._TPC)) {
			Return (\_SB.CP00._TPC)
		} Else {
			Return (0)
		}
	}

	Method (_PTC)
	{
		If (CondRefOf (\_SB.CP00._PTC)) {
			Return (\_SB.CP00._PTC)
		} Else {
			Return (Package ()
			{
				Buffer () { 0 },
				Buffer () { 0 }
			})
		}
	}

	Method (_TSD)
	{
		If (CondRefOf (\_SB.CP00._TSD)) {
			Return (\_SB.CP00._TSD)
		} Else {
			Return (Package ()
			{
				Package () { 5, 0, 0, 0, 0 }
			})
		}
	}

	Method (_TDL)
	{
		If (CondRefOf (\_SB.CP00._TSS)) {
			Store (SizeOf (\_SB.CP00._TSS ()), Local0)
			Local0--
			Return (Local0)
		} Else {
			Return (0)
		}
	}

	/*
	 * Processor Performance Control
	 */

	Method (_PPC)
	{
		Return (0)
	}

	Method (SPPC, 1)
	{
		Store (Arg0, \PPCM)

		/* Notify OS to re-read _PPC limit on each CPU */
		\PPCN ()
	}

	Method (_PSS)
	{
		If (CondRefOf (\_SB.CP00._PSS)) {
			Return (\_SB.CP00._PSS)
		} Else {
			Return (Package ()
			{
				Package () { 0, 0, 0, 0, 0, 0 }
			})
		}
	}

	Method (_PDL)
	{
		/* Check for mainboard specific _PDL override */
		If (CondRefOf (\_SB.MPDL)) {
			Return (\_SB.MPDL)
		} ElseIf (CondRefOf (\_SB.CP00._PSS)) {
			Store (SizeOf (\_SB.CP00._PSS ()), Local0)
			Local0--
			Return (Local0)
		} Else {
			Return (0)
		}
	}

	/* Return PPCC table defined by mainboard */
	Method (PPCC)
	{
		Return (\_SB.MPPC)
	}

#ifdef DPTF_CPU_CRITICAL
	Method (_CRT)
	{
		Return (^^CTOK (DPTF_CPU_CRITICAL))
	}
#endif

#ifdef DPTF_CPU_PASSIVE
	Method (_PSV)
	{
		Return (^^CTOK (DPTF_CPU_PASSIVE))
	}
#endif
}
