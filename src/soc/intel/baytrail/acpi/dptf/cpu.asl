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
	Name (CTYP, 0) /* Passive Cooling by default */

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
		If (CondRefOf (\_PR.CPU0._PSS)) {
			Store (SizeOf (\_PR.CPU0._PSS ()), Local0)
			Decrement (Local0)
			Return (Local0)
		} Else {
			Return (0)
		}
	}

	/*
	 * DPTF
	 */

	/* Convert from Degrees C to 1/10 Kelvin for ACPI */
	Method (CTOK, 1) {
		/* 10th of Degrees C */
		Multiply (Arg0, 10, Local0)

		/* Convert to Kelvin */
		Add (Local0, 2732, Local0)

		Return (Local0)
	}

	/* Critical temperature from NVS */
	Method (_CRT, 0, Serialized)
	{
		Return (CTOK (\TCRT))
	}

	/* Hot temperature is 3 less than critical temperature */
	Method (_HOT, 0, Serialized)
	{
		Return (CTOK (Subtract (\TCRT, 3)))
	}

	Method (_PSV, 0, Serialized)
	{
		If (CTYP) {
			Return (CTOK (\TACT))
		} Else {
			Return (CTOK (\TPSV))
		}
	}

	/* Set Cooling Policy
	 *   Arg0 - Cooling policy mode, 1=active, 0=passive
	 *   Arg1 - Acoustic Limit
	 *   Arg2 - Power Limit
	 */
	Method (_SCP, 3, Serialized)
	{
		If (LEqual (Arg0, 0)) {
			Store (0, CTYP)
		} Else {
			Store (1, CTYP)
		}

		/* DPTF Thermal Trip Points Changed Event */
		Notify (TCPU, 0x91)
	}
}
