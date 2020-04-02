/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
