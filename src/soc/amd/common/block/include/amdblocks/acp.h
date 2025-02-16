/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_COMMON_ACP_H
#define AMD_COMMON_ACP_H

#include <device/device.h>
#include <types.h>

struct acp_config {
	enum {
#if CONFIG(SOC_AMD_COMMON_BLOCK_ACP_GEN2)
		ACP_PINS_HDA_3SDI = 1,		/* HDA 3xSDI */
		ACP_PINS_HDA_1SDI_1SW = 2,	/* HDA 1xSDI,    SW w/Data0 */
		ACP_PINS_4SW_1SW = 3,		/* SW w/Data0-3, SW w/Data0 */
		ACP_PINS_HDA_3SDI_PDM2 = 4,	/* HDA 3xSDI,    PDM 2CH */
		ACP_PINS_HDA_1SDI_PDM6 = 5,	/* HDA 1xSDI,    PDM 6CH */
		ACP_PINS_HDA_1SDI_1SW_PDM2 = 6,	/* HDA 1xSDI,    SW w/Data0, PDM 2CH */
		ACP_PINS_4SW_PDM6 = 7,		/* SW w/Data0-3, PDM 6CH */
		ACP_PINS_4SW_1SW_PDM2 = 8,	/* SW w/Data0-3, SW w/Data0, PDM 2CH */
		ACP_PINS_I2S = 9,		/* 3xI2S, Refclk, Intr */
		ACP_PINS_HDA_3SDI_PDM6_I2S = 10,/* HDA 3xSDI,    PDM 6CH, I2S */
		ACP_PINS_HDA_3SDI_PDM8 = 11,	/* HDA 3xSDI,    PDM 8CH */
		ACP_PINS_HDA_1SDI_1SW_PDM6_I2S = 12,/* HDA 1xSDI, SW w/Data0, PDM 6CH, I2S */
		ACP_PINS_4SW_1SW_PDM6_I2S = 13,	/* SW w/Data0-3, SW w/Data0, PDM 6CH, I2S */
		ACP_PINS_4SW_1SW_PDM8 = 14,	/* SW w/Data0-3, SW w/Data0, PDM 8CH */
#else
		I2S_PINS_MAX_HDA = 0,	/* HDA w/reset  3xSDI, SW w/Data0 */
		I2S_PINS_MAX_MHDA = 1,	/* HDA no reset 3xSDI, SW w/Data0-1 */
		I2S_PINS_MIN_HDA = 2,	/* HDA w/reset  1xSDI, SW w/Data0-2 */
		I2S_PINS_MIN_MHDA = 3,	/* HDA no reset 1xSDI, SW w/Data0-3 */
		I2S_PINS_I2S_TDM = 4,
		I2S_PINS_UNCONF = 7,	/* All pads will be input mode */
#endif
	} acp_pin_cfg;

	/* Enable ACP I2S wake feature (0 = disable, 1 = enable) */
	u8 acp_i2s_wake_enable;
	/* Enable ACP PME (0 = disable, 1 = enable) */
	u8 acp_pme_enable;

	/*
	 * DMIC present (optional) to support ACP DMIC hardware runtime detection on the
	 * platform. If dmic_present is set to true, it will populate the _WOV ACPI method.
	 */
	bool dmic_present;
};

void acp_soc_write_ssdt_entry(const struct device *dev);

#endif /* AMD_COMMON_ACP_H */
