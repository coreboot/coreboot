/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * ThinkPad X61 General Purpose Event handlers
 *
 * _L18 fires on GPIO8 (H8_WAKE#, routed to GPE bit 24 via gpi8_routing=SCI).
 * It reads the 16-bit EC wake-status register and notifies the appropriate
 * ACPI devices.  Bit meanings match the ThinkPad H8 EC protocol:
 *
 *   bit 1 (0x02) - ring indicator wakeup (no ACPI device to notify)
 *   bit 2 (0x04) - lid opened
 *   bit 3 (0x08) - dock event
 *   bit 4 (0x10) - power/sleep button
 *   bit 7 (0x80) - AC adapter status change or other wake
 *
 * Device paths under coreboot's ACPI namespace:
 *   LID  -> \_SB.PCI0.LPCB.EC.LID   (defined in ec/lenovo/h8/acpi/lid.asl)
 *   SLPB -> \_SB.PCI0.LPCB.EC.SLPB  (defined in ec/lenovo/h8/acpi/sleepbutton.asl)
 *   DOCK -> \_SB.DOCK                (defined in acpi/dock.asl)
 */

Scope (\_GPE)
{
	/*
	 * _L18 - EC wake event (GPIO8 / H8_WAKE#, level-triggered SCI)
	 *
	 * Read and dispatch the EC wake source register.  The EC clears the
	 * register after it is read.
	 */
	Method (_L18, 0, NotSerialized)
	{
		Local0 = \_SB.PCI0.LPCB.EC.WAKE

		/* bit 2: Lid opened - wake the system via the lid device */
		If ((Local0 & 0x04)) {
			Notify (\_SB.PCI0.LPCB.EC.LID, 0x02)   // Device Wake
		}

		/* bit 3: Dock connect/disconnect event */
		If ((Local0 & 0x08)) {
			Notify (\_SB.DOCK, 0x03)                // Dock-specific
			Notify (\_SB.PCI0.LPCB.EC.SLPB, 0x02)  // Device Wake
		}

		/* bit 4: Power/sleep button */
		If ((Local0 & 0x10)) {
			Notify (\_SB.PCI0.LPCB.EC.SLPB, 0x02)  // Device Wake
		}

		/* bit 7: Generic wake (AC status change, timer, etc.) */
		If ((Local0 & 0x80)) {
			Notify (\_SB.PCI0.LPCB.EC.SLPB, 0x02)  // Device Wake
		}
	}
}
