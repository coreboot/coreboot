/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <variant/variant.h>
#include <gpio.h>
#include <variant/gpio.h>
#include <baseboard/variants.h>
#include <string.h>

/* Use spd_index array to save mem_id */
static const int spd_index[32] = {
	10, 0, 0, 0, 11, 0, 0, 0,
	0, 0, 0, 0, 9, 0, 0, 0,
	0, 4, 3, 6, 1, 0, 0, 0,
	0, 5, 8, 7, 2, 0, 0, 0
	};

const struct cnl_mb_cfg *get_variant_memory_cfg(struct cnl_mb_cfg *mem_cfg)
{
	int mem_sku;
	struct cnl_mb_cfg baseboard_memcfg  = {
		/*
		* The dqs_map arrays map the ddr4 pins to the SoC pins
		* for both channels.
		*
		* the index = pin number on ddr4 part
		* the value = pin number on SoC
		*/
		.dqs_map[DDR_CH0] = { 0, 1, 4, 5, 2, 3, 6, 7 },
		.dqs_map[DDR_CH1] = { 0, 1, 4, 5, 2, 3, 6, 7 },

		/* Baseboard uses 120, 81 and 100 rcomp resistors */
		.rcomp_resistor = { 120, 81, 100 },

		/* Baseboard Rcomp target values */
		.rcomp_targets = { 100, 40, 20, 20, 26 },

		/* Set CaVref config to 2 */
		.vref_ca_config = 2,

		/* Enable Early Command Training */
		.ect = 1,
	};

	mem_sku = variant_memory_sku();

	memcpy(mem_cfg, &baseboard_memcfg, sizeof(baseboard_memcfg));

	/* In Drallion dual channel is enabled by default.
	 * spd[0]-spd[3] map to CH0D0, CH0D1, CH1D0, Ch1D1 respectively.
	 * Dual-DIMM memory is not used in drallion family, so we only
	 * fill in spd info for CH0D0 and CH1D0 here.
	 */
	for (int i = 0; i < 3; i = i+2) {
		mem_cfg->spd[i].read_type = READ_SPD_CBFS;
		mem_cfg->spd[i].spd_spec.spd_index = mem_sku;
	}

	return mem_cfg;
}

int variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
		GPIO_MEM_CONFIG_4,
	};

	return spd_index[gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios))];
}
