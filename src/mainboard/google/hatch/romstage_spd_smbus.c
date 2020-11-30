/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <spd_cache.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	struct cnl_mb_cfg memcfg;
	variant_memory_params(&memcfg);

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
		memcfg.spd[0].spd_spec.spd_data_ptr_info.spd_data_ptr = (uintptr_t)blk.spd_array[0];
	}

	memcfg.spd[1].read_type = NOT_EXISTING;

	if (blk.spd_array[1] == NULL) {
		memcfg.spd[2].read_type = NOT_EXISTING;
	} else {
		memcfg.spd[2].read_type = READ_SPD_MEMPTR;
		memcfg.spd[2].spd_spec.spd_data_ptr_info.spd_data_len = blk.len;
		memcfg.spd[2].spd_spec.spd_data_ptr_info.spd_data_ptr = (uintptr_t)blk.spd_array[1];
	}

	memcfg.spd[3].read_type = NOT_EXISTING;
	dump_spd_info(&blk);

	/* set to 2 VREF_CA goes to CH_A and VREF_DQ_B goes to CH_B. */
	memcfg.vref_ca_config = 2;
	memcfg.dq_pins_interleaved = 1;

	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
