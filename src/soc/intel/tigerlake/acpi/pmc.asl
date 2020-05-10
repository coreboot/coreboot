/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/iomap.h>

Scope (\_SB.PCI0) {

	Device (PMC)
	{
		Name (_HID, "INTC1026")
		Name (_DDN, "Intel(R) Tiger Lake IPC Controller")
		/*
		 * PCH preserved 32 MB MMIO range from 0xFC800000 to 0xFE7FFFFF.
		 * 64KB (0xFE000000 - 0xFE00FFFF) for PMC MBAR.
		 */
		Name (_CRS, ResourceTemplate () {
			Memory32Fixed (ReadWrite, PCH_PWRM_BASE_ADDRESS, PCH_PWRM_BASE_SIZE)
		})

		/* The OS mux driver will be bound to this device node. */
		Device (MUX)
		{
			Name (_HID, "INTC105C")
			Name (_DDN, "Intel(R) Tiger Lake North Mux-Agent")
		}
	}
}
