/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <memory_info.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <variant/gpio.h>

/*
 * GPIO_MEM_CH_SEL is set to 1 for single channel skus
 * and 0 for dual channel skus.
 */
#define GPIO_MEM_CH_SEL		GPP_F2

int __weak variant_memory_sku(void)
{
	const gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	struct cnl_mb_cfg memcfg;
	int mem_sku;
	int is_single_ch_mem;

	variant_memory_params(&memcfg);
	mem_sku = variant_memory_sku();
	/*
	 * GPP_F2 is the MEM_CH_SEL gpio, which is set to 1 for single
	 * channel skus and 0 for dual channel skus.
	 */
	is_single_ch_mem = gpio_get(GPIO_MEM_CH_SEL);

	/*
	 * spd[0]-spd[3] map to CH0D0, CH0D1, CH1D0, CH1D1 respectively.
	 * Dual-DIMM memory is not used in hatch family, so we only
	 * fill in spd_info for CH0D0 and CH1D0 here.
	 */
	memcfg.spd[0].read_type = READ_SPD_CBFS;
	memcfg.spd[0].spd_spec.spd_index = mem_sku;
	if (!is_single_ch_mem) {
		memcfg.spd[2].read_type = READ_SPD_CBFS;
		memcfg.spd[2].spd_spec.spd_index = mem_sku;
	}

	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
