/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
			Memory32Fixed (ReadWrite, PCH_PWRM_BASE_ADDRESS, 0x00010000)
		})
	}
}
