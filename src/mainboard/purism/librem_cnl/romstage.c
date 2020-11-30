/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <spd_cache.h>
#include "variant.h"

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *mem_cfg = &memupd->FspmConfig;

	struct cnl_mb_cfg memcfg = {

		/*
		* Rcomp resistor values.  These values represent the resistance in
		* ohms of the three rcomp resistors attached to the DDR_COMP_0,
		* DDR_COMP_1, and DDR_COMP_2 pins on the DRAM.
		*/
		.rcomp_resistor = { 121, 81, 100 },

		/* Rcomp target values */
		.rcomp_targets = { 100, 40, 20, 20, 26 },

		/*
		* Indicates whether memory is interleaved.
		* Set to 1 for an interleaved design,
		* set to 0 for non-interleaved design.
		*/
		.dq_pins_interleaved = 1,

		/*
		* VREF_CA configuration.
		* Set to 0 VREF_CA goes to both CH_A and CH_B,
		* set to 1 VREF_CA goes to CH_A and VREF_DQ_A goes to CH_B,
		* set to 2 VREF_CA goes to CH_A and VREF_DQ_B goes to CH_B.
		*/
		.vref_ca_config = 2,

		/* Early Command Training */
		.ect = 0,
	};


	/* Read spd block to get memory config */
	struct spd_block blk = {
		.addr_map = { 0x50, 0x52, },
	};

	uint8_t *spd_cache;
	size_t spd_cache_sz;
	bool need_update_cache = false;
	bool dimm_changed = true;

	/* load spd cache from RW_SPD_CACHE */
	if (load_spd_cache(&spd_cache, &spd_cache_sz) == CB_SUCCESS) {
		if (!spd_cache_is_valid(spd_cache, spd_cache_sz)) {
			printk(BIOS_WARNING, "Invalid SPD cache\n");
		} else {
			dimm_changed = check_if_dimm_changed(spd_cache, &blk);
			if (dimm_changed && memupd->FspmArchUpd.NvsBufferPtr != 0) {
				/* Set mrc_cache as invalid */
				printk(BIOS_INFO, "Set mrc_cache as invalid\n");
				memupd->FspmArchUpd.NvsBufferPtr = 0;
			}
		}
		need_update_cache = true;
	}

	if (!dimm_changed) {
		spd_fill_from_cache(spd_cache, &blk);
	} else {
		/* Access memory info through SMBUS. */
		get_spd_smbus(&blk);

		if (need_update_cache && update_spd_cache(&blk) == CB_ERR)
			printk(BIOS_WARNING, "update SPD cache failed\n");
	}

	if (blk.spd_array[0] == NULL) {
		memcfg.spd[0].read_type = NOT_EXISTING;
	} else {
		memcfg.spd[0].read_type = READ_SPD_MEMPTR;
		memcfg.spd[0].spd_spec.spd_data_ptr_info.spd_data_len = blk.len;
		memcfg.spd[0].spd_spec.spd_data_ptr_info.spd_data_ptr =
						(uintptr_t)blk.spd_array[0];
	}

	memcfg.spd[1].read_type = NOT_EXISTING;

	if (blk.spd_array[1] == NULL) {
		memcfg.spd[2].read_type = NOT_EXISTING;
	} else {
		memcfg.spd[2].read_type = READ_SPD_MEMPTR;
		memcfg.spd[2].spd_spec.spd_data_ptr_info.spd_data_len = blk.len;
		memcfg.spd[2].spd_spec.spd_data_ptr_info.spd_data_ptr =
						(uintptr_t)blk.spd_array[1];
	}

	memcfg.spd[3].read_type = NOT_EXISTING;
	dump_spd_info(&blk);

	cannonlake_memcfg_init(mem_cfg, &memcfg);
	variant_memory_init_params(mem_cfg);
}
