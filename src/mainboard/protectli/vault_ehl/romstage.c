/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>
#include <soc/meminit.h>
#include <spd_bin.h>
#include "gpio.h"

static const struct mb_cfg ddr4_mem_config = {
	.UserBd = BOARD_TYPE_MOBILE,
	.dq_pins_interleaved = 0,
	.vref_ca_config = 2,
	.ect = 0,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *mem_cfg = &memupd->FspmConfig;

	struct spd_info module_spd_info = {
		.read_type = READ_SPD_MEMPTR,
	};

	/* The only DIMM slot is routed to Channel 1 */
	struct spd_block blk = {
		.addr_map = { 0x52 },
	};

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	get_spd_smbus(&blk);
	dump_spd_info(&blk);

	if (blk.spd_array[0] == NULL)
		die("No memory detected. Insert DIMM module");

	module_spd_info.spd_spec.spd_data_ptr_info.spd_data_ptr = (uintptr_t)blk.spd_array[0];
	module_spd_info.spd_spec.spd_data_ptr_info.spd_data_len = (uintptr_t)blk.len;

	/* Set half_populated as false, because DIMM is on Channel 1 */
	memcfg_init(mem_cfg, &ddr4_mem_config, &module_spd_info, false);

	/* Clear Channel 0 DIMM 0 SPD, as the slot is not populated */
	mem_cfg->MemorySpdPtr00 = 0;
	/* Return back to coreboot if something goes wrong */
	mem_cfg->ExitOnFailure = 1;

	/* Disable both DIMMs at Channel 0 */
	mem_cfg->DisableDimmChannel0 = 3;
	/* Disable DIMM 1 at Channel 1 */
	mem_cfg->DisableDimmChannel1 = 2;

	mem_cfg->Lp4DqsOscEn = 0;
}
