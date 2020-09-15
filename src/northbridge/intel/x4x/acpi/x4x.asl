/* SPDX-License-Identifier: GPL-2.0-only */

#include "hostbridge.asl"
#include "../memmap.h"
#include <southbridge/intel/common/rcba.h>

/* PCI Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 1)

	Name (PDRS, ResourceTemplate() {
		Memory32Fixed(ReadWrite, DEFAULT_RCBA, 0x00004000)
		Memory32Fixed(ReadWrite, DEFAULT_MCHBAR,   0x00004000)
		Memory32Fixed(ReadWrite, DEFAULT_DMIBAR,   0x00001000)
		Memory32Fixed(ReadWrite, DEFAULT_EPBAR,    0x00001000)
		Memory32Fixed(ReadWrite, CONFIG_MMCONF_BASE_ADDRESS, 0x04000000)
		Memory32Fixed(ReadWrite, 0xfed20000, 0x00020000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed40000, 0x00005000) // Misc ICH
		Memory32Fixed(ReadWrite, 0xfed45000, 0x0004b000) // Misc ICH
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		Return(PDRS)
	}
}

// PCIe graphics port 0:1.0
#include "peg.asl"
