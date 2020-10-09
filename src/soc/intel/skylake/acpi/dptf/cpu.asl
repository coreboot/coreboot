/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DPTF_CPU_PASSIVE
#define DPTF_CPU_PASSIVE        80
#endif

#ifndef DPTF_CPU_CRITICAL
#define DPTF_CPU_CRITICAL       90
#endif

External (\_SB.CP00._PSS, PkgObj)
External (\_SB.MPDL, IntObj)

Device (B0D4)
{
	Name(_ADR, 0x00040000)  /* Bus 0, Device 4, Function 0 */

	Method (_STA)
	{
		If (\DPTE == 1) {
			Return (0xF)
		} Else {
			Return (0x0)
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
		\PPCM = Arg0

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
			Local0 = SizeOf (\_SB.CP00._PSS)
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
		Return (\_SB.DPTF.CTOK (DPTF_CPU_CRITICAL))
	}
#endif

#ifdef DPTF_CPU_PASSIVE
	Method (_PSV)
	{
		Return (\_SB.DPTF.CTOK (DPTF_CPU_PASSIVE))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC0
	Method (_AC0)
	{
		Return (\_SB.DPTF.CTOK (DPTF_CPU_ACTIVE_AC0))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC1
	Method (_AC1)
	{
		Return (\_SB.DPTF.CTOK (DPTF_CPU_ACTIVE_AC1))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC2
	Method (_AC2)
	{
		Return (\_SB.DPTF.CTOK (DPTF_CPU_ACTIVE_AC2))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC3
	Method (_AC3)
	{
		Return (\_SB.DPTF.CTOK (DPTF_CPU_ACTIVE_AC3))
	}
#endif

#ifdef DPTF_CPU_ACTIVE_AC4
	Method (_AC4)
	{
		Return (\_SB.DPTF.CTOK (DPTF_CPU_ACTIVE_AC4))
	}
#endif
}
