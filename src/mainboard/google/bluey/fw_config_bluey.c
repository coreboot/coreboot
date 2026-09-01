/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fw_config.h>
#include <soc/cdt.h>
#include <soc/pcie.h>
#include <soc/platform_info.h>

void fw_config_get_mainboard_override(uint64_t *fw_config)
{
	if (!CONFIG(SOC_QUALCOMM_CDT))
		return;

	enum qclib_soc_id soc = platform_get_soc_id();
	uint16_t soc_id;

	switch (soc) {
	case SOC_ID_HAMOA:
		soc_id = HAMOA_ID_SCP;
		break;
	case SOC_ID_X1P42100:
		soc_id = X1P42100_ID_SCP;
		break;
	default:
		printk(BIOS_WARNING, "CDT: Unknown SoC ID, skipping fw_config override\n");
		return;
	}

	uint16_t platform_id = cdt_get_platform_id();

	*fw_config = CDT_COMBINE_SOC_PLATFORM_ID(soc_id, platform_id);

	uint8_t storage_type = (soc == SOC_ID_X1P42100) ?
			       platform_get_fast_boot() : CALYPSO_STORAGE_TYPE_NVME;

	fw_config_value_set_field(fw_config, FW_CONFIG_FIELD(STORAGE_TYPE), storage_type);

	printk(BIOS_INFO, "CDT: soc_id=0x%04x platform_id=0x%04x storage_type=%u\n",
	       soc_id, platform_id, storage_type);
}

bool mainboard_needs_pcie_init(void)
{
	return fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME));
}
