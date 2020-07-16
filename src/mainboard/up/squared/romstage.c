/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <soc/romstage.h>
#include <soc/gpio_apl.h>
#include <soc/meminit.h>
#include <fsp/api.h>
#include <FspmUpd.h>
#include <console/console.h>
#include <gpio.h>
#include "gpio.h"

/*
 * Offsets:
 *  - GPIO_214: 0xd8
 *  - GPIO_215: 0xe0
 */
static const uint8_t memory_skuid_pads[] = { GPIO_214, GPIO_215 };

static const struct lpddr4_sku skus[] = {
	/* Samsung 280 K4F8E304HB-MGCJ 8Gb dual-ch */
	[0] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = 0,
		.ch0_dual_rank = 0,
		.ch1_dual_rank = 0,
		.part_num = "K4F8E304HB-MGCJ",
	},
	[1] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_8Gb_DENSITY,
		.ch1_rank_density = LP4_8Gb_DENSITY,
		.ch0_dual_rank = 0,
		.ch1_dual_rank = 0,
		.part_num = "K4F8E304HB-MGCJ",
	},
	[2] = {
		.speed = LP4_SPEED_2400,
		.ch0_rank_density = LP4_16Gb_DENSITY,
		.ch1_rank_density = LP4_16Gb_DENSITY,
		.ch0_dual_rank = 1,
		.ch1_dual_rank = 1,
		.part_num = "K4F6E304HB-MGCJ",
	},
};

static const struct lpddr4_cfg lp4cfg = {
	.skus = skus,
	.num_skus = ARRAY_SIZE(skus),
};

static const uint8_t ch0_bit_swizzling[] = {
	0x0D, 0x0A, 0x08, 0x0B, 0x0C, 0x0F, 0x0E, 0x09,
	0x06, 0x00, 0x03, 0x04, 0x07, 0x01, 0x05, 0x02,
	0x1C, 0x1A, 0x19, 0x1B, 0x1D, 0x1F, 0x1E, 0x18,
	0x10, 0x17, 0x15, 0x16, 0x14, 0x12, 0x13, 0x11
};

static const uint8_t ch1_bit_swizzling[] = {
	0x00, 0x07, 0x04, 0x05, 0x06, 0x02, 0x03, 0x01,
	0x08, 0x0F, 0x0D, 0x0B, 0x0A, 0x09, 0x0E, 0x0C,
	0x17, 0x11, 0x13, 0x12, 0x14, 0x15, 0x16, 0x10,
	0x1C, 0x1A, 0x1D, 0x1F, 0x18, 0x19, 0x1E, 0x1B
};

static const uint8_t ch2_bit_swizzling[] = {
	0x0D, 0x08, 0x0B, 0x0E, 0x0C, 0x0F, 0x09, 0x0A,
	0x04, 0x07, 0x01, 0x06, 0x02, 0x03, 0x00, 0x05,
	0x18, 0x19, 0x1C, 0x1A, 0x1D, 0x1E, 0x1F, 0x1B,
	0x11, 0x13, 0x15, 0x10, 0x16, 0x12, 0x17, 0x14
};

static const uint8_t ch3_bit_swizzling[] = {
	0x00, 0x05, 0x04, 0x07, 0x03, 0x02, 0x06, 0x01,
	0x0A, 0x0B, 0x08, 0x09, 0x0C, 0x0E, 0x0D, 0x0F,
	0x12, 0x16, 0x14, 0x13, 0x17, 0x11, 0x15, 0x10,
	0x19, 0x1F, 0x1D, 0x1B, 0x1E, 0x18, 0x1C, 0x1A
};

/*
 *  GPIO215  GPIO214  Memory size
 *     0        0       2 GiB
 *     0        1       4 GiB
 *     1        0       8 GiB
 *     1        1       8 GiB
 */
static uint8_t get_memory_skuid(void)
{
	uint8_t memory_skuid = 0;

	for (uint8_t i = 0; i < ARRAY_SIZE(memory_skuid_pads); i++) {
		uint8_t rx_state = gpio_get(memory_skuid_pads[i]);
		memory_skuid |= rx_state << i;
	}
	return memory_skuid;
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	printk(BIOS_DEBUG, "MAINBOARD: %s/%s called\n", __FILE__, __func__);

	FSP_M_CONFIG *config = &memupd->FspmConfig;

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
	if (CONFIG(MINI_PCIE_MODE_MSATA))
		gpio_configure_pads(msata_mode_gpio_table, ARRAY_SIZE(msata_mode_gpio_table));

	uint8_t memory_skuid = get_memory_skuid();
	printk(BIOS_DEBUG, "MAINBOARD: Found memory SKU ID: 0x%02x\n", memory_skuid);

	switch (memory_skuid) {
	case 0: /* 2GB */
		config->DualRankSupportEnable = 0;
		config->Ch0_RankEnable        = 1;
		config->Ch0_DramDensity       = 2;
		config->Ch1_RankEnable        = 1;
		config->Ch1_DramDensity       = 2;
		config->Ch2_RankEnable        = 0;
		config->Ch3_RankEnable        = 0;
		printk(BIOS_INFO, "MAINBOARD: Found supported memory: 2GB\n");
		break;
	case 1: /* 4GB */
		config->DualRankSupportEnable = 1;
		config->Ch0_RankEnable        = 1;
		config->Ch0_DramDensity       = 2;
		config->Ch1_RankEnable        = 1;
		config->Ch1_DramDensity       = 2;
		config->Ch2_RankEnable        = 1;
		config->Ch2_DramDensity       = 2;
		config->Ch3_RankEnable        = 1;
		config->Ch3_DramDensity       = 2;
		printk(BIOS_INFO, "MAINBOARD: Found supported memory: 4GB\n");
		break;
	case 2: /* 8GB */
		config->DualRankSupportEnable = 1;
		config->Ch0_RankEnable        = 3;
		config->Ch0_DramDensity       = 2;
		config->Ch1_RankEnable        = 3;
		config->Ch1_DramDensity       = 2;
		config->Ch2_RankEnable        = 3;
		config->Ch2_DramDensity       = 2;
		config->Ch3_RankEnable        = 3;
		config->Ch3_DramDensity       = 2;
		printk(BIOS_INFO, "MAINBOARD: Found supported memory: 8GB\n");
		break;
	case 3: /* 8GB */
		config->DualRankSupportEnable = 1;
		config->Ch0_RankEnable        = 1;
		config->Ch0_DramDensity       = 4;
		config->Ch1_RankEnable        = 1;
		config->Ch1_DramDensity       = 4;
		config->Ch2_RankEnable        = 1;
		config->Ch2_DramDensity       = 4;
		config->Ch3_RankEnable        = 1;
		config->Ch3_DramDensity       = 4;
		printk(BIOS_INFO, "MAINBOARD: Found supported memory: 8GB\n");
		break;
	default:
		printk(BIOS_INFO, "MAINBOARD: No supported memory found!\n");
		break;
	}

	config->Package = 0x1;			// 0x0
	config->Profile = 0xB;			// 0x19
	config->MemoryDown = 0x1;		// 0x0
	config->DDR3LPageSize = 0x0;		// 0x1
	config->DIMM0SPDAddress = 0x0;		// 0xa0
	config->DIMM1SPDAddress = 0x0;		// 0xa4
	config->RmtCheckRun = 0x3;		// 0x0
	config->RmtMarginCheckScaleHighThreshold = 0xC8;	// 0x0
	config->EnhancePort8xhDecoding = 0x0;	// 0x1

	config->Ch0_DeviceWidth = 0x1;		// 0x0
	config->Ch0_Option = 0x3;		// 0x0
	config->Ch1_DeviceWidth = 0x1;		// 0x0
	config->Ch1_Option = 0x3;		// 0x0
	config->Ch2_DeviceWidth = 0x1;		// 0x0
	config->Ch2_Option = 0x3;		// 0x0
	config->Ch3_DeviceWidth = 0x1;		// 0x0
	config->Ch3_Option = 0x3;		// 0x0
	config->StartTimerTickerOfPfetAssert = 0x4E20;	// 0x0

	memcpy(config->Ch0_Bit_swizzling, &ch0_bit_swizzling,
			sizeof(ch0_bit_swizzling));
	memcpy(config->Ch1_Bit_swizzling, &ch1_bit_swizzling,
			sizeof(ch1_bit_swizzling));
	memcpy(config->Ch2_Bit_swizzling, &ch2_bit_swizzling,
			sizeof(ch2_bit_swizzling));
	memcpy(config->Ch3_Bit_swizzling, &ch3_bit_swizzling,
			sizeof(ch3_bit_swizzling));
}

void mainboard_save_dimm_info(void)
{
	save_lpddr4_dimm_info(&lp4cfg, get_memory_skuid());
}
