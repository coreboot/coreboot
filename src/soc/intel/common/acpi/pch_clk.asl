/* SPDX-License-Identifier: GPL-2.0-or-later */

#define PCR_BIOS_BUFFEN 0x8080

Scope (\_SB)
{
	/* MTL PCH CLK */
	Device (ICLK) {
		Name (_HID, EISAID ("PNP0C02"))
		Name (_UID, "SOCCLK")

		Method (_STA)
		{
			/*
			 * Device is present, enabled and decoding its resources
			 * and should not be shown in UI
			 */
			Return (0x3)
		}

		/*
		 * PCIe(100MHz) clock disable
		 * Arg0 - clock index
		 */
		Method (CLKD, 1) {
			\_SB.PCI0.PCRA (PID_ISCLK, PCR_BIOS_BUFFEN, Not (ShiftLeft (1, Arg0)))
		}

		/*
		 * PCIe(100MHz) clock enable
		 * Arg0 - clock index
		 */
		Method (CLKE, 1) {
			\_SB.PCI0.PCRO (PID_ISCLK, PCR_BIOS_BUFFEN, (ShiftLeft (1, Arg0)))
		}
	}
}
