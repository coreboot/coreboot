/* SPDX-License-Identifier: GPL-2.0-only */

	/*
	 * Store the lid state in Opregion "Mailbox 1 Public ACPI Methods"
	 * (CLID), without notifying the driver. Use this before the
	 * driver is loaded, e.g. when the EC comes up.
	 *
	 * @param Arg0 Lid state, 0 = closed, 1 = open
	 * @Return 0 on success, 1 if there is no Opregion or mailbox 1
	 */
	Method (SLID, 1, Serialized)
	{
		If (ASLS == 0) {
			Return (1)
		}
		If ((^BOX3.MBOX & 1) == 0) {
			Return (1)
		}

		If (Arg0) {
			^BOX3.CLID = 3
		} Else {
			^BOX3.CLID = 0
		}
		Return (0)
	}

	/*
	 * Report a lid state change to the driver, same as the Intel
	 * reference GLID/GNOT: store CLID, then send a lid event (CEVT 2).
	 * The Windows driver uses this, not the ACPI lid device. i915
	 * reads the lid from _LID and ignores the event.
	 *
	 * @param Arg0 Lid state, 0 = closed, 1 = open
	 * @Return 0 on success, 1 on failure
	 * Errors: * ASLS is zero or mailbox 1 support not advertised
	 *         * Driver not ready, CLID is marked pending (bit 31)
	 */
	Method (GLID, 1, Serialized)
	{
		If (SLID (Arg0) != 0) {
			Return (1)
		}

		If (^BOX3.DRDY == 0) {
			^BOX3.CLID |= 0x80000000
			Return (1)
		}

		^BOX3.CEVT = 2	/* Lid event */
		^BOX3.CSTS = 3	/* BIOS dispatched an event */
		/* Re-enumerate only if the driver doesn't handle hotplug */
		If (^BOX3.CHPD == 0) {
			Notify (GFX0, 0)
		}
		Notify (GFX0, 0x80)
		Return (0)
	}
