/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	static struct spd_info ehlcrb_spd_info;
	const struct mb_cfg *board_cfg = variant_memcfg_config();

	/* TODO: Read the resistor strap to get number of memory segments */
	bool half_populated = false;
	/* Initialize spd information for LPDDR4x board */
	ehlcrb_spd_info.read_type = READ_SPD_CBFS;
	ehlcrb_spd_info.spd_spec.spd_index = 0x00;

	/* Initialize variant specific configurations */
	memcfg_init(&memupd->FspmConfig, board_cfg, &ehlcrb_spd_info, half_populated);
}
