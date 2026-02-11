/* SPDX-License-Identifier: GPL-2.0-only */
/* ACP Audio Configuration */
Scope (\_SB.PCI0.GP41) {
	Device (ACPD) {
		/* Device addressing for ACP (Audio Coprocessor) */
		Name (_ADR, 0x05)  /* Device 0, Function 5 */

		Name (STAT, 0x3) /* Decoding Resources, Hide from UI */
		Method (_STA, 0x0, NotSerialized)
		{
			Return (STAT)
		}
	}
}
