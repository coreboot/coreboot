/* SPDX-License-Identifier: GPL-2.0-only */

// TODO: Does board actually support DPTF?
#include "thermal.asl"

Scope (_SB)
{
	Method (MPTS, 1, NotSerialized)  // _PTS: Prepare To Sleep
	{
		^PCI0.LPCB.EC0.ECPS (Arg0)
		/* TBT and DTS not supported, TPM.PTS can be called elsewhere */
	}

	Method (MWAK, 1, Serialized)  // _WAK: Wake
	{
		^PCI0.LPCB.EC0.ECWK (Arg0)
		/* No GPIO expander, 8254 clock-gating and PCIe PME can be performed elsewhere */

		If ((Arg0 == 3) || (Arg0 == 4))
		{
			/* DTS and TBT not supported, iGFX RC variable update stripped */
			LIDS = ^LID0._LID ()
			Notify (LID0, 0x80) // Status Change
			/* TODO: Bus checks? Based on KabylakeOpenBoardPkg - Platform.asl
			   perhaps not (Warm insertion/removal not possible on mobile */
		}
	}

	Method (MS0X, 1, Serialized)	// S0ix hook. Porting "GUAM" method - "Global User Absent Mode"
	{
		If (Arg0 == 0)
		{
			/* Exit "Connected Standby" */
#if 1	// EC Notification
			^PCI0.LPCB.EC0.EOSS = 0
#endif
			/* TODO: P-state capping, PL setting? */
		}
		ElseIf (Arg0 == 1)
		{
			/* Enter "Connected Standby" */
#if 1	// EC Notification
			^PCI0.LPCB.EC0.ECSS = 0x08
#endif
			/* TODO: P-state capping, PL setting? */
		}
	}

	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D") /* Lid Device */)
		Method (_LID, 0, NotSerialized)  // _LID: Lid Status
		{
#if CONFIG(EC_USE_LGMR)
			Return (^^PCI0.LPCB.EC0.MLID)
#else
			Return (^^PCI0.LPCB.EC0.ELID)
#endif
		}

		Method (_PSW, 1, NotSerialized)  // _PSW: Power State Wake
		{
			^^PCI0.LPCB.EC0.EIDW = Arg0
		}

		Name (_PRW, Package () { 0x0A, 3 })  // _PRW: Power Resources for Wake
	}

	Device (SLPB)
	{
		Name (_HID, EisaId ("PNP0C0E") /* Sleep Button Device */)
		Name (_PRW, Package () { 0x0A, 3 })  // _PRW: Power Resources for Wake
	}
}

Scope (_GPE)
{
	/* TODO - Remaining Level-Triggered GPEs: PCH GPE, PCIe PME, TBT, DTS, GFX SCI and tier-2 (RTD3) */
	Method (_L0A, 0, NotSerialized)
	{
		Notify (\_SB.SLPB, 0x02) // Device Wake
	}
}
