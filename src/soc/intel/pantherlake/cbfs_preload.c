/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cbfs.h>
#include <fsp/api.h>
#include <intelblocks/cfg.h>
#include <program_loading.h>

static void preload_pre_fsps(void *unused)
{
	preload_fsps();

	struct soc_intel_common_config *config = chip_get_common_soc_structure();
	/* If chipset lockdown is handled by FSP, CBFS pre-loading must be completed
	   before FSP-S finishes. In this case, it yields better results to preload the
	   payload before lockdown.*/
	if (config->chipset_lockdown == CHIPSET_LOCKDOWN_FSP)
		payload_preload();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, preload_pre_fsps, NULL);

static void preload_post_fsps(void *unused)
{
	struct soc_intel_common_config *config = chip_get_common_soc_structure();
	/* If the chipset lockdown is handled by FSP, SPI DMA is locked, and we cannot
	   preload any other CBFS files. */
	if (config->chipset_lockdown == CHIPSET_LOCKDOWN_FSP)
		return;

	cbfs_preload(CONFIG_CBFS_PREFIX "/dsdt.aml");
	payload_preload();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_ENTRY, preload_post_fsps, NULL);
