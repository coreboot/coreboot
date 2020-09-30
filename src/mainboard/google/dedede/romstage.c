/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <memory_info.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <string.h>
#include <variant/gpio.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *board_cfg = variant_memcfg_config();
	const struct spd_info spd_info = {
		.read_type = READ_SPD_CBFS,
		.spd_spec.spd_index = variant_memory_sku(),
	};
	bool half_populated = variant_mem_is_half_populated();

	memcfg_init(&memupd->FspmConfig, board_cfg, &spd_info, half_populated);
}

const char *mainboard_get_dram_part_num(void)
{
	static char part_num_store[DIMM_INFO_PART_NUMBER_SIZE];

	if (google_chromeec_cbi_get_dram_part_num(&part_num_store[0],
					sizeof(part_num_store)) < 0) {
		printk(BIOS_ERR, "No DRAM part number in CBI!\n");
		return NULL;
	}

	return part_num_store;
}
