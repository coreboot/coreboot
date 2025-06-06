/* SPDX-License-Identifier: GPL-2.0-or-later */

#define PCR_BIOS_BUFFEN 0x8080

/* IOE PCR access */
#include <soc/intel/common/acpi/ioe_pcr.asl>

Scope (\_SB)
{
	/* MTL IOE CLK */
	Device (ECLK) {
		Name (_HID, EISAID ("PNP0C02"))
		Name (_UID, "IOECLK")

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
			\_SB.PCI0.ICRA (PID_ISCLK, PCR_BIOS_BUFFEN, Not (ShiftLeft (1, Arg0)))
		}

		/*
		 * PCIe(100MHz) clock enable
		 * Arg0 - clock index
		 */
		Method (CLKE, 1) {
			\_SB.PCI0.ICRO (PID_ISCLK, PCR_BIOS_BUFFEN, (ShiftLeft (1, Arg0)))
		}
	}
}
