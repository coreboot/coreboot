External (\_PR.CPU0._TSS, MethodObj)
External (\_PR.CPU0._TPC, MethodObj)
External (\_PR.CPU0._PTC, PkgObj)
External (\_PR.CPU0._TSD, PkgObj)
External (\_PR.CPU0._PPC, MethodObj)
External (\_PR.CPU0._PSS, MethodObj)

Device (TCPU)
{
	Name (_HID, EISAID ("INT3401"))
	Name (_UID, 0)

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
		If (CondRefOf (\_PR.CPU0._PPC)) {
			Return (\_PR.CPU0._PPC)
		} Else {
			Return (0)
		}
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

	Name (PPCC, Package ()
	{
		0x2,		// Revision
		Package () {	// Power Limit 1
			0,	// PowerLimitIndex, 0 for Power Limit 1
			1600,	// PowerLimitMinimum
			6200,	// PowerLimitMaximum
			1000,	// TimeWindowMinimum
			1000,	// TimeWindowMaximum
			200	// StepSize
		},
		Package () {	// Power Limit 2
			1,	// PowerLimitIndex, 1 for Power Limit 2
			8000,	// PowerLimitMinimum
			8000,	// PowerLimitMaximum
			1000,	// TimeWindowMinimum
			1000,	// TimeWindowMaximum
			1000	// StepSize
		}
	})

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
