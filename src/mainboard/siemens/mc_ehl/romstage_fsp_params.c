/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <device/dram/common.h>
#include <device/mmio.h>
#include <hwilib.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <string.h>
#include <types.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	static struct spd_info spd_info;
	const struct mb_cfg *board_cfg = variant_memcfg_config();
	static uint8_t spd_data[CONFIG_DIMM_SPD_SIZE];
	const char *cbfs_hwi_name = "hwinfo.hex";

	/* Initialize SPD information for LPDDR4x from HW-Info primarily with a fallback to
	   spd.bin in the case where the SPD data in HW-Info is not available or invalid. */
	memset(spd_data, 0, sizeof(spd_data));
	if ((hwilib_find_blocks(cbfs_hwi_name) == CB_SUCCESS) &&
	    (hwilib_get_field(SPD, spd_data, 0x80) == 0x80) &&
	    (ddr_crc16(spd_data, 126) == read16((void *)&spd_data[126]))) {
		spd_info.spd_spec.spd_data_ptr_info.spd_data_ptr = (uintptr_t)spd_data;
		spd_info.spd_spec.spd_data_ptr_info.spd_data_len = CONFIG_DIMM_SPD_SIZE;
		spd_info.read_type = READ_SPD_MEMPTR;
	} else {
		die("SPD in HW-Info not valid!\n");
	}
	/* Initialize variant specific configurations */
	memcfg_init(&memupd->FspmConfig, board_cfg, &spd_info, false);

	/* Enable Row-Hammer prevention */
	memupd->FspmConfig.RhPrevention = 1;
}
