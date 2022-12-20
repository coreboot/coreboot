/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <variant/gpio.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	size_t pads_num;

	meminit_lpddr4_by_sku(&memupd->FspmConfig,
		variant_lpddr4_config(), variant_memory_sku());

	pads = variant_romstage_gpio_table(&pads_num);
	gpio_configure_pads(pads, pads_num);
}

void mainboard_save_dimm_info(void)
{
	save_lpddr4_dimm_info(variant_lpddr4_config(), variant_memory_sku());
}
