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
 *
 */

#ifndef _MAINBOARD_EMMC_H
#define _MAINBOARD_EMMC_H

#include <fsp/util.h>

#define DEFAULT_EMMC_DLL_SIGN 0x55aa

#ifndef __ACPI__
BL_EMMC_INFORMATION harcuvar_emmc_config[] = {
	/*
	 *  Default eMMC DLL configuration.
	 */
	{DEFAULT_EMMC_DLL_SIGN,
	 {0x00000508, 0x00000c11, 0x1c2a2a2a, 0x00191e27, 0x00000a0a,
	  0x00010013, 0x00000001} } };
#endif

#endif /* _MAINBOARD_EMMC_H */
