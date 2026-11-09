/* SPDX-License-Identifier: GPL-2.0-only */
#include "board.h"
#include <boardid.h>
#include <console/console.h>
#include <fw_config.h>
#include <soc/pcie.h>
#include <soc/platform_info.h>

void fw_config_mainboard_override(void)
{
	uint64_t fw_config_soc_id = FW_CONFIG_VALUE(SOC_ID, HAMOA);
	uint32_t raw_soc_id = soc_id();
	uint16_t soc_hw_id = raw_soc_id & SOC_ID_HW_MASK;

	switch (soc_hw_id) {
	case TCSR_SOC_HW_VERSION_DEVICE_NUM_HAMOA:
		fw_config_soc_id = FW_CONFIG_VALUE(SOC_ID, HAMOA);
		break;
	case TCSR_SOC_HW_VERSION_DEVICE_NUM_X1P42100:
		if (raw_soc_id == CANIM_SOC_ID)
			fw_config_soc_id = FW_CONFIG_VALUE(SOC_ID, CANIM);
		else
			fw_config_soc_id = FW_CONFIG_VALUE(SOC_ID, X1P42100);
		break;
	default:
		printk(BIOS_WARNING, "Unknown SoC ID detected: 0x%x\n", soc_hw_id);
	}

	fw_config_override_field(FW_CONFIG_FIELD(SOC_ID), fw_config_soc_id);
}

bool mainboard_needs_pcie_init(void)
{
#if ENV_RAMSTAGE
	/* Skip PCIe initialization if boot mode is "low-battery" or "off-mode charging"*/
	if (is_low_power_boot_with_charger())
		return false;
#endif

	if (CONFIG(EC_GOOGLE_CHROMEEC_FW_CONFIG_FROM_UFSC)) {
		return !(fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_UFS)));
	}

	return !(CONFIG(MAINBOARD_HAS_UFS));
}
