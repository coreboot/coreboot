/*
 * This file is part of the coreboot project.
 *
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

#include <baseboard/variants.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <spd_bin.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	struct cnl_mb_cfg memcfg;
	variant_memory_params(&memcfg);

	/* Read spd block to get memory config */
	struct spd_block blk = {
		.addr_map = { 0x50, 0x52, },
	};

	/* Access memory info through SMBUS. */
	get_spd_smbus(&blk);
	memcfg.spd[0].read_type = READ_SPD_MEMPTR;
	memcfg.spd[0].spd_spec.spd_data_ptr_info.spd_data_len = blk.len;
	memcfg.spd[0].spd_spec.spd_data_ptr_info.spd_data_ptr = (uintptr_t)blk.spd_array[0];

	memcfg.spd[1].read_type = NOT_EXISTING;

	memcfg.spd[2].read_type = READ_SPD_MEMPTR;
	memcfg.spd[2].spd_spec.spd_data_ptr_info.spd_data_len = blk.len;
	memcfg.spd[2].spd_spec.spd_data_ptr_info.spd_data_ptr = (uintptr_t)blk.spd_array[1];

	memcfg.spd[3].read_type = NOT_EXISTING;
	dump_spd_info(&blk);

	/* set to 2 VREF_CA goes to CH_A and VREF_DQ_B goes to CH_B. */
	memcfg.vref_ca_config = 2;
	memcfg.dq_pins_interleaved = 1;

	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
