/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/cpu.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <intelblocks/mp_init.h>
#include <variant/gpio.h>

/* DQ byte map */
static const u8 dq_map[][12] = {
	{ 0x0F, 0xF0, 0x0F, 0xF0, 0xFF, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0F, 0xF0, 0x0F, 0xF0, 0xFF, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

/* DQS CPU<>DRAM map */
static const u8 dqs_map[][8] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0, 1, 2, 3, 5, 4, 7, 6 },
};

/* Rcomp resistor */
static const u16 rcomp_resistor[] = { 100, 100, 100 };

void __weak variant_memory_params(struct lpddr4_config *mem_config)
{
	/* Rcomp target */
	static const u16 rcomp_target_es0[] = { 80, 40, 40, 40, 30 };
	static const u16 rcomp_target_es1[] = { 60, 20, 20, 20, 20 };

	mem_config->dq_map = dq_map;
	mem_config->dq_map_size = sizeof(dq_map);
	mem_config->dqs_map = dqs_map;
	mem_config->dqs_map_size = sizeof(dqs_map);
	mem_config->rcomp_resistor = rcomp_resistor;
	mem_config->rcomp_resistor_size = sizeof(rcomp_resistor);
	if (cpu_get_cpuid() == CPUID_ICELAKE_A0) {
		mem_config->rcomp_target = rcomp_target_es0;
		mem_config->rcomp_target_size = sizeof(rcomp_target_es0);
	} else {
		mem_config->rcomp_target = rcomp_target_es1;
		mem_config->rcomp_target_size = sizeof(rcomp_target_es1);
	}
}

int __weak variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}
