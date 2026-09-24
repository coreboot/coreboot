/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <console/console.h>
#include <fw_config.h>
#include <soc/cdt.h>
#include <soc/pcie.h>
#include <soc/platform_info.h>

void fw_config_mainboard_override(void)
{
	if (!CONFIG(SOC_QUALCOMM_CDT))
		return;

	uint32_t raw_soc_id = soc_id();
	uint16_t soc_hw_id = raw_soc_id & SOC_ID_HW_MASK;
	uint16_t cdt_soc_id;

	switch (soc_hw_id) {
	case TCSR_SOC_HW_VERSION_DEVICE_NUM_HAMOA:
		cdt_soc_id = FW_CONFIG_VALUE(SOC_ID, HAMOA);
		break;
	case TCSR_SOC_HW_VERSION_DEVICE_NUM_X1P42100:
		switch (raw_soc_id) {
		case CANIM_SOC_ID:
			cdt_soc_id = FW_CONFIG_VALUE(SOC_ID, CANIM);
			break;
		default:
			cdt_soc_id = FW_CONFIG_VALUE(SOC_ID, X1P42100);
			break;
		}
		break;
	default:
		printk(BIOS_WARNING, "CDT: Unknown SoC ID, skipping fw_config override\n");
		return;
	}

	uint16_t platform_id = cdt_get_platform_id();
	uint8_t storage_type = (soc_hw_id == TCSR_SOC_HW_VERSION_DEVICE_NUM_X1P42100) ?
			       platform_get_fast_boot() : CALYPSO_STORAGE_TYPE_NVME;

	fw_config_override_field(FW_CONFIG_FIELD(PLATFORM_ID), platform_id);
	fw_config_override_field(FW_CONFIG_FIELD(SOC_ID), cdt_soc_id);
	fw_config_override_field(FW_CONFIG_FIELD(STORAGE_TYPE), storage_type);
}

bool mainboard_needs_pcie_init(void)
{
	return fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME));
}
