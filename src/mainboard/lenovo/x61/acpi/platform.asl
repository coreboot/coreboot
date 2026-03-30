/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * ThinkPad X61 platform sleep/wake hooks
 *
 * _PTS (Prepare To Sleep) - called by the OS before entering a sleep state.
 * _WAK (Wake)             - called by the OS after resuming from sleep.
 *
 * Vendor BIOS reference: Phoenix BIOS TP-7N DSDT _PTS / _WAK methods.
 *
 * EC method availability (ec/lenovo/h8/acpi/ec.asl):
 *   MUTE(n)       - audio mute (n=1 mute, n=0 unmute)
 *   USBP(n)       - USB power (n=0 disable, n=1 enable)
 *   RADI(n)       - wireless radios (n=0 off, n=1 on)
 *
 * HKEY method availability (ec/lenovo/h8/acpi/thinkpad.asl):
 *   MHKC(n)       - enable (n=1) / disable (n=0) all HKEY hotkey events
 *   WAKE(n)       - re-initialise BT/WWAN state after wake (called with sleep type)
 */

/*
 * _PTS - Prepare To Sleep
 * Arg0: target sleep state (1=S1, 3=S3, 4=S4, 5=S5)
 */
Method (_PTS, 1)
{
	/* Mute audio to avoid pops on suspend */
	\_SB.PCI0.LPCB.EC.MUTE (1)

	/* Disable USB bus power during sleep */
	\_SB.PCI0.LPCB.EC.USBP (0)

	/* Turn off wireless radios */
	\_SB.PCI0.LPCB.EC.RADI (0)

	/* Disable HKEY hotkey events to prevent spurious wakes */
	\_SB.PCI0.LPCB.EC.HKEY.MHKC (0)
}

/*
 * _WAK - System Wake
 * Arg0: sleep state we are resuming from
 */
Method (_WAK, 1)
{
	/* Re-enable HKEY hotkey events */
	\_SB.PCI0.LPCB.EC.HKEY.MHKC (1)

	/* Re-initialise BT/WWAN radio state (restores pre-sleep radio enables) */
	\_SB.PCI0.LPCB.EC.HKEY.WAKE (Arg0)

	Return (Package () { 0, 0 })
}

/* ------------------------------------------------------------------ */
/* System Bus - _INI                                                   */
/* ------------------------------------------------------------------ */

Scope (\_SB)
{
	/*
	 * Placed at the top level so it runs first, before any device _INI.
	 * Calls \GOS() to detect the running OS and set OS-compatibility flags
	 * used by other ACPI methods.
	 */
	Method (_INI, 0)
	{
		\GOS ()
	}
}
