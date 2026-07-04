/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Lenovo VPC ("VPC2004") feature/hotkey device, consumed by the Linux
 * ideapad-laptop driver for Fn hotkeys, rfkill, camera, conservation mode,
 * etc.
 *
 * The driver reads the capability bitmask by evaluating VPC2004._CFG. _CFG
 * is a vendor reserved name that iASL rejects under -we (warning 3133), so
 * it is emitted at runtime via acpigen from the mainboard (ideapad_fill_ssdt
 * in ramstage.c) and returns VPCF.
 *
 * The capability bits follow the ideapad-laptop driver definition:
 *   bit 16 Bluetooth, bit 17 WWAN/3G, bit 18 WiFi, bit 19 camera,
 *   bits 8-10 keyboard-backlight type.
 *
 * NOTE: the VPCR/VPCW command mailbox (the EC-RAM command/data handshake the
 * driver drives through those methods for reads/writes) still requires
 * clean-room reverse-engineering of the live EC command sequence and is
 * intentionally omitted for now. Core functions (battery, AC, lid) do not
 * depend on it.
 */

Device (VPC0)
{
	Name (_HID, "VPC2004")
	Name (_UID, Zero)

	/* Capability bitmask returned by _CFG (emitted via acpigen). */
	Name (VPCF, 0xF60F0015)

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0F)
	}
}
