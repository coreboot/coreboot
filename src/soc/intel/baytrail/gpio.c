/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <device/pci.h>
#include <compiler.h>
#include <console/console.h>
#include <soc/gpio.h>
#include <soc/pmc.h>
#include <soc/smm.h>

/* GPIO-to-Pad LUTs */
static const u8 gpncore_gpio_to_pad[GPNCORE_COUNT] =
	{ 19, 18, 17, 20, 21, 22, 24, 25,	/* [ 0: 7] */
	  23, 16, 14, 15, 12, 26, 27,  1,	/* [ 8:15] */
	   4,  8, 11,  0,  3,  6, 10, 13,	/* [16:23] */
	   2,  5,  9 };				/* [24:26] */

static const u8 gpscore_gpio_to_pad[GPSCORE_COUNT] =
	{  85,  89, 93,  96, 99, 102,  98, 101,	/* [ 0:  7] */
	   34,  37, 36,  38, 39,  35,  40,  84,	/* [ 8: 15] */
	   62,  61, 64,  59, 54,  56,  60,  55,	/* [16: 23] */
	   63,  57, 51,  50, 53,  47,  52,  49,	/* [24: 31] */
	   48,  43, 46,  41, 45,  42,  58,  44,	/* [32: 39] */
	   95, 105, 70,  68, 67,  66,  69,  71,	/* [40: 47] */
	   65,  72, 86,  90, 88,  92, 103,  77,	/* [48: 55] */
	   79,  83, 78,  81, 80,  82,  13,  12,	/* [56: 63] */
	   15,  14, 17,  18, 19,  16,   2,   1,	/* [64: 71] */
	    0,   4,  6,   7,  9,   8,  33,  32,	/* [72: 79] */
	   31,  30, 29,  27, 25,  28,  26,  23,	/* [80: 87] */
	   21,  20, 24,  22,  5,   3,  10,  11,	/* [88: 95] */
	  106,  87, 91, 104, 97, 100 };		/* [96:101] */

static const u8 gpssus_gpio_to_pad[GPSSUS_COUNT] =
	{ 29, 33, 30, 31, 32, 34, 36, 35,	/* [ 0: 7] */
	  38, 37, 18,  7, 11, 20, 17,  1,	/* [ 8:15] */
	   8, 10, 19, 12,  0,  2, 23, 39,	/* [16:23] */
	  28, 27, 22, 21, 24, 25, 26, 51,	/* [24:31] */
	  56, 54, 49, 55, 48, 57, 50, 58,	/* [32:39] */
	  52, 53, 59, 40 };			/* [40:43] */

/* GPIO bank descriptions */
static const struct gpio_bank gpncore_bank = {
	.gpio_count = GPNCORE_COUNT,
	.gpio_to_pad = gpncore_gpio_to_pad,
	.legacy_base = GP_LEGACY_BASE_NONE,
	.pad_base = GPNCORE_PAD_BASE,
	.has_wake_en = 0,
	.gpio_f1_range_start = GPNCORE_GPIO_F1_RANGE_START,
	.gpio_f1_range_end = GPNCORE_GPIO_F1_RANGE_END,
};

static const struct gpio_bank gpscore_bank = {
	.gpio_count = GPSCORE_COUNT,
	.gpio_to_pad = gpscore_gpio_to_pad,
	.legacy_base = GPSCORE_LEGACY_BASE,
	.pad_base = GPSCORE_PAD_BASE,
	.has_wake_en = 0,
	.gpio_f1_range_start = GPSCORE_GPIO_F1_RANGE_START,
	.gpio_f1_range_end = GPSCORE_GPIO_F1_RANGE_END,
};

static const struct gpio_bank gpssus_bank = {
	.gpio_count = GPSSUS_COUNT,
	.gpio_to_pad = gpssus_gpio_to_pad,
	.legacy_base = GPSSUS_LEGACY_BASE,
	.pad_base = GPSSUS_PAD_BASE,
	.has_wake_en = 1,
	.gpio_f1_range_start = GPSSUS_GPIO_F1_RANGE_START,
	.gpio_f1_range_end = GPSSUS_GPIO_F1_RANGE_END,
};

static void setup_gpios(const struct soc_gpio_map *gpios,
			const struct gpio_bank *bank)
{
	const struct soc_gpio_map *config;
	int gpio = 0;
	u32 reg, pad_conf0;
	u8 set, bit;

	u32 use_sel[4] = {0};
	u32 io_sel[4] = {0};
	u32 gp_lvl[4] = {0};
	u32 tpe[4] = {0};
	u32 tne[4] = {0};
	u32 wake_en[4] = {0};

	if (!gpios)
		return;

	for (config = gpios; config->pad_conf0 != GPIO_LIST_END;
	     config++, gpio++) {
		if (gpio > bank->gpio_count)
			break;

		set = gpio >> 5;
		bit = gpio % 32;

		if (bank->legacy_base != GP_LEGACY_BASE_NONE) {
			/* Legacy IO configuration */
			use_sel[set] |= config->use_sel << bit;
			io_sel[set]  |= config->io_sel  << bit;
			gp_lvl[set]  |= config->gp_lvl  << bit;
			tpe[set]     |= config->tpe     << bit;
			tne[set]     |= config->tne     << bit;

			/* Some banks do not have wake_en ability */
			if (bank->has_wake_en)
				wake_en[set] |= config->wake_en << bit;
		}

		/* Pad configuration registers */
		reg = bank->pad_base + 16 * bank->gpio_to_pad[gpio];

		/* Add correct func to GPIO pad config */
		pad_conf0 = config->pad_conf0;
		if (config->is_gpio)
		{
			if (gpio >= bank->gpio_f1_range_start &&
			    gpio <= bank->gpio_f1_range_end)
				pad_conf0 |= PAD_FUNC1;
			else
				pad_conf0 |= PAD_FUNC0;
		}

#ifdef GPIO_DEBUG
		printk(BIOS_DEBUG, "Write Pad: Base(%x) - %x %x %x\n",
		       reg, pad_conf0, config->pad_conf1, config->pad_val);
#endif

		write32((u32 *)(reg + PAD_CONF0_REG), pad_conf0);
		write32((u32 *)(reg + PAD_CONF1_REG), config->pad_conf1);
		write32((u32 *)(reg + PAD_VAL_REG), config->pad_val);
	}

	if (bank->legacy_base != GP_LEGACY_BASE_NONE)
		for (set = 0; set <= (bank->gpio_count - 1) / 32; ++set) {
			reg = bank->legacy_base + 0x20 * set;

#ifdef GPIO_DEBUG
			printk(BIOS_DEBUG,
			       "Write GPIO: Reg(%x) - %x %x %x %x %x\n",
				reg, use_sel[set], io_sel[set], gp_lvl[set],
				tpe[set], tne[set]);
#endif

			outl(use_sel[set], reg + LEGACY_USE_SEL_REG);
			outl(io_sel[set], reg + LEGACY_IO_SEL_REG);
			outl(gp_lvl[set], reg + LEGACY_GP_LVL_REG);
			outl(tpe[set], reg + LEGACY_TPE_REG);
			outl(tne[set], reg + LEGACY_TNE_REG);

			/* TS registers are WOC  */
			outl(0, reg + LEGACY_TS_REG);

			if (bank->has_wake_en)
				outl(wake_en[set], reg + LEGACY_WAKE_EN_REG);
		}
}

static void setup_gpio_route(const struct soc_gpio_map *sus,
                             const struct soc_gpio_map *core)
{
	uint32_t route_reg = 0;
	int i;

	for (i = 0; i < 8; i++) {
		/* SMI takes precedence and wake_en implies SCI. */
		if (sus[i].smi) {
			route_reg |= ROUTE_SMI << (2 * i);
		} else if (sus[i].sci) {
			route_reg |= ROUTE_SCI << (2 * i);
		}

		if (core[i].smi) {
			route_reg |= ROUTE_SMI << (2 * (i + 8));
		} else if (core[i].sci) {
			route_reg |= ROUTE_SCI << (2 * (i + 8));
		}
	}
	southcluster_smm_save_param(SMM_SAVE_PARAM_GPIO_ROUTE, route_reg);
}

static void setup_dirqs(const u8 dirq[GPIO_MAX_DIRQS],
			const struct gpio_bank *bank)
{
	u32 *reg = (u32 *)(bank->pad_base + PAD_BASE_DIRQ_OFFSET);
	u32 val;
	int i;

	/* Write all four DIRQ registers */
	for (i=0; i<4; ++i) {
		val = dirq[i * 4 + 3] << 24 | dirq[i * 4 + 2] << 16 |
		      dirq[i * 4 + 1] << 8  | dirq[i * 4];
		write32(reg + i, val);
#ifdef GPIO_DEBUG
		printk(BIOS_DEBUG, "Write DIRQ reg(%x) - %x\n",
			reg + i, val);
#endif
	}
}

void setup_soc_gpios(struct soc_gpio_config *config, u8 enable_xdp_tap)
{
	if (config) {
		setup_gpios(config->ncore, &gpncore_bank);
		setup_gpios(config->score, &gpscore_bank);
		setup_gpios(config->ssus,  &gpssus_bank);
		setup_gpio_route(config->ssus, config->score);

		if (config->core_dirq)
			setup_dirqs(*config->core_dirq, &gpscore_bank);
		if (config->sus_dirq)
			setup_dirqs(*config->sus_dirq, &gpssus_bank);
	}

	/* Set on die termination feature with pull up value and
	 * drive the pad high for TAP_TDO and TAP_TMS
	 */
	if (!enable_xdp_tap) {
		printk(BIOS_DEBUG, "Tri-state TDO and TMS\n");
		write32((u32 *)(GPSSUS_PAD_BASE + 0x2fc), 0xc);
		write32((u32 *)(GPSSUS_PAD_BASE + 0x2cc), 0xc);
	}
}

struct soc_gpio_config* __weak mainboard_get_gpios(void)
{
	printk(BIOS_DEBUG, "Default/empty GPIO config\n");
	return NULL;
}
