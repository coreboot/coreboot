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
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <fsp/api.h>
#include <FspmUpd.h>
#include <baseboard/variants.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	const struct lpddr4_swizzle_cfg *cfg;
	const struct lpddr4_chan_swizzle_cfg *chan;
	uint8_t spd[0x80];
	size_t num;

	/* setup early gpio before memory */
	pads = variant_early_gpio_table(&num);
	gpio_configure_pads(pads, num);

	/*
	 * Get DRAM configuration data from hwinfo block.
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

	/*
	 * Some of the mc_apl1 boards use LPDDR4 memory. In this case, the
	 * correct swizzle configuration is necessary. The default settings
	 * for swizzling are 0, since the baseboard does not use LPDDR4 memory.
	 */
	cfg = variant_lpddr4_swizzle_config();

	/*
	 * CH0_DQB byte lanes in the bit swizzle configuration field are
	 * not 1:1. The mapping within the swizzling field is:
	 *   indices [0:7]   - byte lane 1 (DQS1) DQ[8:15]
	 *   indices [8:15]  - byte lane 0 (DQS0) DQ[0:7]
	 *   indices [16:23] - byte lane 3 (DQS3) DQ[24:31]
	 *   indices [24:31] - byte lane 2 (DQS2) DQ[16:23]
	 */
	chan = &cfg->phys[LP4_PHYS_CH0B];
	memcpy(&memupd->FspmConfig.Ch0_Bit_swizzling[0], &chan->dqs[LP4_DQS1],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch0_Bit_swizzling[8], &chan->dqs[LP4_DQS0],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch0_Bit_swizzling[16], &chan->dqs[LP4_DQS3],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch0_Bit_swizzling[24], &chan->dqs[LP4_DQS2],
			(size_t)DQ_BITS_PER_DQS);

	/* CH0_DQA byte lanes in the bit swizzle configuration field are 1:1. */
	chan = &cfg->phys[LP4_PHYS_CH0A];
	memcpy(&memupd->FspmConfig.Ch1_Bit_swizzling[0], &chan->dqs[LP4_DQS0],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch1_Bit_swizzling[8], &chan->dqs[LP4_DQS1],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch1_Bit_swizzling[16], &chan->dqs[LP4_DQS2],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch1_Bit_swizzling[24], &chan->dqs[LP4_DQS3],
			(size_t)DQ_BITS_PER_DQS);

	/*
	 * CH1_DQB byte lanes in the bit swizzle configuration field are
	 * not 1:1. The mapping within the swizzling field is:
	 *   indices [0:7]   - byte lane 1 (DQS1) DQ[8:15]
	 *   indices [8:15]  - byte lane 0 (DQS0) DQ[0:7]
	 *   indices [16:23] - byte lane 3 (DQS3) DQ[24:31]
	 *   indices [24:31] - byte lane 2 (DQS2) DQ[16:23]
	 */
	chan = &cfg->phys[LP4_PHYS_CH1B];
	memcpy(&memupd->FspmConfig.Ch2_Bit_swizzling[0], &chan->dqs[LP4_DQS1],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch2_Bit_swizzling[8], &chan->dqs[LP4_DQS0],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch2_Bit_swizzling[16], &chan->dqs[LP4_DQS3],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch2_Bit_swizzling[24], &chan->dqs[LP4_DQS2],
			(size_t)DQ_BITS_PER_DQS);

	/* CH1_DQA byte lanes in the bit swizzle configuration field are 1:1. */
	chan = &cfg->phys[LP4_PHYS_CH1A];
	memcpy(&memupd->FspmConfig.Ch3_Bit_swizzling[0], &chan->dqs[LP4_DQS0],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch3_Bit_swizzling[8], &chan->dqs[LP4_DQS1],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch3_Bit_swizzling[16], &chan->dqs[LP4_DQS2],
			(size_t)DQ_BITS_PER_DQS);
	memcpy(&memupd->FspmConfig.Ch3_Bit_swizzling[24], &chan->dqs[LP4_DQS3],
			(size_t)DQ_BITS_PER_DQS);

	memupd->FspmConfig.MsgLevelMask = 0x0;
	memupd->FspmConfig.MrcDataSaving = 0x0;
	memupd->FspmConfig.MrcFastBoot   = 0x1;
}
