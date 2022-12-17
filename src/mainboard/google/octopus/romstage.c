/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <boardid.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <memory_info.h>
#include <soc/gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	size_t pads_num;

	meminit_lpddr4_by_sku(&memupd->FspmConfig,
		variant_lpddr4_config(), variant_memory_sku());

	pads = variant_romstage_gpio_table(&pads_num);
	gpio_configure_pads(pads, pads_num);
}

static void save_dimm_info_by_sku_config(void)
{
	save_lpddr4_dimm_info(variant_lpddr4_config(), variant_memory_sku());
}

void mainboard_save_dimm_info(void)
{
	char part_num_store[DIMM_INFO_PART_NUMBER_SIZE];
	const char *part_num = NULL;

	if (CONFIG(DRAM_PART_NUM_NOT_ALWAYS_IN_CBI)) {
		/* Fall back on part numbers encoded in lp4cfg array. */
		if ((int)board_id() < CONFIG_DRAM_PART_IN_CBI_BOARD_ID_MIN) {
			save_dimm_info_by_sku_config();
			return;
		}
	}

	if (google_chromeec_cbi_get_dram_part_num(&part_num_store[0],
			ARRAY_SIZE(part_num_store)) < 0)
		printk(BIOS_ERR, "Couldn't obtain DRAM part number from CBI\n");
	else
		part_num = &part_num_store[0];

	save_lpddr4_dimm_info_part_num(part_num);
}
