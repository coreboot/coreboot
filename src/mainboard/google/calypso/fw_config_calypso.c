/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fw_config.h>
#include <soc/cdt.h>
#include <soc/platform_info.h>

void fw_config_get_mainboard_override(uint64_t *fw_config)
{
	uint16_t soc_id;
	switch (platform_get_soc_id()) {
	case SOC_ID_CALYPSO:
		soc_id = CALYPSO_ID_SCP;
		break;
	default:
		printk(BIOS_WARNING, "CDT: Unknown SoC ID, skipping fw_config override\n");
		return;
	}

	uint16_t platform_id = cdt_get_platform_id();
	uint8_t storage_type = platform_get_fast_boot();

	*fw_config = CDT_COMBINE_SOC_PLATFORM_ID(soc_id, platform_id);

	fw_config_value_set_field(fw_config, FW_CONFIG_FIELD(STORAGE_TYPE), storage_type);

	printk(BIOS_INFO, "CDT: soc_id=0x%04x platform_id=0x%04x storage_type=%u\n",
	       soc_id, platform_id, storage_type);
}
