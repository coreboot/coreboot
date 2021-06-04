/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __FSP_GUIDS__
#define __FSP_GUIDS__

#include <uuid.h>

#define AMD_FSP_TSEG_HOB_GUID \
		GUID_INIT(0x5fc7897a, 0x5aff, 0x4c61, \
		0xaa, 0x7a, 0xdd, 0xcf, 0xa9, 0x18, 0x43, 0x0c)

#define AMD_FSP_ACPI_ALIB_HOB_GUID \
		GUID_INIT(0x42494c41, 0x4002, 0x403b, \
		0x87, 0xE1, 0x3F, 0xEB, 0x13, 0xC5, 0x66, 0x9A)

#define AMD_FSP_PCIE_DEVFUNC_REMAP_HOB_GUID \
		GUID_INIT(0X6D5CD69D, 0XFB24, 0X4461, \
		0XAA, 0X32, 0X8E, 0XE1, 0XB3, 0X3, 0X31, 0X9C )

#endif /* __FSP_GUIDS__ */
