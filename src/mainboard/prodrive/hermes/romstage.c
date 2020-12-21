/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <variant/variants.h>
#include "variants/baseboard/include/eeprom.h"

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	memupd->FspmConfig.UserBd = 7;
	memupd->FspmTestConfig.SmbusSpdWriteDisable = 0;
	memupd->FspmConfig.IedSize = 0x400000;
	cannonlake_memcfg_init(&memupd->FspmConfig, variant_memcfg_config());

	/* Overwrite memupd */
	if (!check_signature(offsetof(struct eeprom_layout, mupd), FSPM_UPD_SIGNATURE))
		return;

	READ_EEPROM_FSP_M(memupd, FspmConfig.RMT);
	READ_EEPROM_FSP_M(memupd, FspmConfig.HyperThreading);
	READ_EEPROM_FSP_M(memupd, FspmConfig.BootFrequency);
	READ_EEPROM_FSP_M(memupd, FspmTestConfig.VtdDisable);
}
