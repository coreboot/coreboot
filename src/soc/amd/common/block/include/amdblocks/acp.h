/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_COMMON_ACP_H
#define AMD_COMMON_ACP_H

struct acp_config {
	enum {
		I2S_PINS_MAX_HDA = 0,	/* HDA w/reset  3xSDI, SW w/Data0 */
		I2S_PINS_MAX_MHDA = 1,	/* HDA no reset 3xSDI, SW w/Data0-1 */
		I2S_PINS_MIN_HDA = 2,	/* HDA w/reset  1xSDI, SW w/Data0-2 */
		I2S_PINS_MIN_MHDA = 3,	/* HDA no reset 1xSDI, SW w/Data0-3 */
		I2S_PINS_I2S_TDM = 4,
		I2S_PINS_UNCONF = 7,	/* All pads will be input mode */
	} acp_pin_cfg;

	/* Enable ACP I2S wake feature (0 = disable, 1 = enable) */
	u8 acp_i2s_wake_enable;
	/* Enable ACP PME (0 = disable, 1 = enable) */
	u8 acp_pme_enable;

	/* If using an external 48MHz OSC for codec, will disable internal X48M_OSC */
	bool acp_i2s_use_external_48mhz_osc;
};

#endif /* AMD_COMMON_ACP_H */
