/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 * Copyright (C) 2017-2018 Siemens AG
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

#include <console/console.h>
#include <hwilib.h>
#include <lib.h>
#include <string.h>
#include <soc/romstage.h>
#include <fsp/api.h>
#include <FspmUpd.h>
#include <baseboard/variants.h>

static const uint8_t Ch0_Bit_swizzling[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t Ch1_Bit_swizzling[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t Ch2_Bit_swizzling[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t Ch3_Bit_swizzling[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	uint8_t spd[0x80];
	size_t num;

	/* setup early gpio before memory */
	pads = variant_early_gpio_table(&num);
	gpio_configure_pads(pads, num);

	/* Get DRAM configuration data from hwinfo block.
	 * The configuration data from hwinfo block is a one-to-one
	 * representation of the FSPM_UPD data starting with parameter
	 * 'Package' (offset 0x4d) and ending before parameter
	 * 'Ch0_Bit_swizzling' (offset 0x88).
	 */
	if (hwilib_find_blocks("hwinfo.hex")) {
		printk(BIOS_ERR,
			"HWInfo not found, use default values for FSP-M.\n");
		return;
	}

	if (hwilib_get_field(SPD, spd, sizeof(spd)) != sizeof(spd)) {
		printk(BIOS_ERR,
			"SPD not found in HWInfo, use defaults for FSP-M.\n");
		return;
	}

	memcpy(&memupd->FspmConfig.Package, &spd,
			(((uint8_t *)memupd->FspmConfig.Ch0_Bit_swizzling)-
			(&memupd->FspmConfig.Package)));

	memcpy(memupd->FspmConfig.Ch0_Bit_swizzling, &Ch0_Bit_swizzling,
		sizeof(Ch0_Bit_swizzling));
	memcpy(memupd->FspmConfig.Ch1_Bit_swizzling, &Ch1_Bit_swizzling,
		sizeof(Ch1_Bit_swizzling));
	memcpy(memupd->FspmConfig.Ch2_Bit_swizzling, &Ch2_Bit_swizzling,
		sizeof(Ch2_Bit_swizzling));
	memcpy(memupd->FspmConfig.Ch3_Bit_swizzling, &Ch3_Bit_swizzling,
		sizeof(Ch3_Bit_swizzling));

	memupd->FspmConfig.MsgLevelMask = 0x0;
	memupd->FspmConfig.MrcDataSaving = 0x0;
	memupd->FspmConfig.MrcFastBoot   = 0x1;
}
