External (\_PR.CPU0._TSS, MethodObj)
External (\_PR.CPU0._TPC, MethodObj)
External (\_PR.CPU0._PTC, PkgObj)
External (\_PR.CPU0._TSD, PkgObj)
External (\_PR.CPU0._PSS, MethodObj)
External (\_SB.DPTF.CTOK, MethodObj)

Device (B0DB)
{
	Name (_ADR, 0x000B0000)  /* Bus 0, Device B, Function 0 */

	Method (_STA)
	{
		If (LEqual (\DPTE, One)) {
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
		If (CondRefOf (\_PR.CPU0._TSS)) {
			Return (\_PR.CPU0._TSS)
		} Else {
			Return (Package ()
			{
				Package () { 0, 0, 0, 0, 0 }
			})
		}
	}

	Method (_TPC)
	{
		If (CondRefOf (\_PR.CPU0._TPC)) {
			Return (\_PR.CPU0._TPC)
		} Else {
			Return (0)
		}
	}

	Method (_PTC)
	{
		If (CondRefOf (\_PR.CPU0._PTC)) {
			Return (\_PR.CPU0._PTC)
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
		If (CondRefOf (\_PR.CPU0._TSD)) {
			Return (\_PR.CPU0._TSD)
		} Else {
			Return (Package ()
			{
				Package () { 5, 0, 0, 0, 0 }
			})
		}
	}

	Method (_TDL)
	{
		If (CondRefOf (\_PR.CPU0._TSS)) {
			Store (SizeOf (\_PR.CPU0._TSS ()), Local0)
			Decrement (Local0)
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
		If (CondRefOf (\_PR.CPU0._PSS)) {
			Return (\_PR.CPU0._PSS)
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
		} ElseIf (CondRefOf (\_PR.CPU0._PSS)) {
			Store (SizeOf (\_PR.CPU0._PSS ()), Local0)
			Decrement (Local0)
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
		Return (\_SB.DPTF.CTOK(DPTF_CPU_CRITICAL))
	}
#endif

#ifdef DPTF_CPU_PASSIVE
	Method (_PSV)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_PASSIVE))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC0
	Method (_AC0)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC0))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC1
	Method (_AC1)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC1))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC2
	Method (_AC2)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC2))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC3
	Method (_AC3)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC3))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC4
	Method (_AC4)
	{
		Return (\_SB.DPTF.CTOK(DPTF_CPU_ACTIVE_AC4))
	}
#endif
}
