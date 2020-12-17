/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <variant/variants.h>
#include "variants/baseboard/include/eeprom.h"

static fsp_params parmas_list[] = {
	GET_VALUE(RMT),
	GET_VALUE(HyperThreading),
	GET_VALUE(BootFrequency)
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	memupd->FspmConfig.UserBd = 7;
	memupd->FspmTestConfig.SmbusSpdWriteDisable = 0;
	memupd->FspmConfig.IedSize = 0x400000;
	cannonlake_memcfg_init(&memupd->FspmConfig, variant_memcfg_config());

	/* Overwrite memupd */
	if (!check_signature(EEPROM_OFFSET_FSP_SIGNATURE, FSP_UPD_SIGNATURE))
		return;

	for (size_t i = 0; i < ARRAY_SIZE(parmas_list); i++) {
		read_write_config(memupd, EEPROM_OFFSET_FSP_CONFIG +
			parmas_list[i].offset,
			EEPROM_OFFSET_FSP_CONFIG + parmas_list[i].offset,
			parmas_list[i].size);
	}
}
